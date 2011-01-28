/*!
** Copyright 2008, Google Inc.
**
** Licensed under the Apache License, Version 2.0 (the "License");
** you may not use this file except in compliance with the License.
** You may obtain a copy of the License at
**
**     http://www.apache.org/licenses/LICENSE-2.0
**
** Unless required by applicable law or agreed to in writing, software
** distributed under the License is distributed on an "AS IS" BASIS,
** WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
** See the License for the specific language governing permissions and
** limitations under the License.
*/

//#define LOG_NDEBUG 0
#define LOG_TAG "QualcommCameraHardware"
#define CONFIG_SENSOR_M4MO
#include <utils/Log.h>

#include "QualcommCameraHardware.h"



#include <utils/threads.h>
#include <binder/MemoryHeapPmem.h>
#include <utils/String16.h>
#include <sys/types.h>
#include <sys/stat.h>

//#include "jpeg_encoder.h"

#include <unistd.h>
#if HAVE_ANDROID_OS
#include <linux/android_pmem.h>
#endif
#include <linux/ioctl.h>

#define LIKELY(exp)   __builtin_expect(!!(exp), 1)
#define UNLIKELY(exp) __builtin_expect(!!(exp), 0)

extern "C" {
#include "exifwriter.h"
  
#include <fcntl.h>
#include <time.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>
#include <assert.h>
#include <stdlib.h>
#include <ctype.h>
#include <signal.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/time.h>
#include <stdlib.h>

#include "sec_m4mo.h"

#define THUMBNAIL_WIDTH        192 //512
#define THUMBNAIL_HEIGHT       144 //384
#define THUMBNAIL_WIDTH_STR    "192" //"512"
#define THUMBNAIL_HEIGHT_STR  "144" //"384"
#define DEFAULT_PICTURE_WIDTH  1280 // 1280
#define DEFAULT_PICTURE_HEIGHT 960 // 768
#define THUMBNAIL_BUFFER_SIZE (THUMBNAIL_WIDTH * THUMBNAIL_HEIGHT * 3/2)

#define DEFAULT_PREVIEW_SETTING 3 // HVGA
#define PREVIEW_SIZE_COUNT (sizeof(preview_sizes)/sizeof(preview_size_type))

#define NOT_FOUND -1

#include <dlfcn.h>

void* (*LINK_cam_conf)(void *data);
void* (*LINK_cam_frame)(void *data);
bool  (*LINK_jpeg_encoder_init)();
void  (*LINK_jpeg_encoder_join)();
unsigned char (*LINK_jpeg_encoder_encode)(const char* file_name, const cam_ctrl_dimension_t *dimen, 
					  const unsigned char* thumbnailbuf, int thumbnailfd,
					  const unsigned char* snapshotbuf, int snapshotfd, common_crop_t *cropInfo) ;
				  
int  (*LINK_camframe_terminate)(void);
int8_t (*LINK_jpeg_encoder_setMainImageQuality)(uint32_t quality);
int8_t (*LINK_jpeg_encoder_setThumbnailQuality)(uint32_t quality);
int8_t (*LINK_jpeg_encoder_setRotation)(uint32_t rotation);
int8_t (*LINK_jpeg_encoder_setLocation)(const camera_position_type *location);
// callbacks
void  (**LINK_mmcamera_camframe_callback)(struct msm_frame_t *frame);
void  (**LINK_mmcamera_jpegfragment_callback)(uint8_t *buff_ptr,
                                              uint32_t buff_size);
void  (**LINK_mmcamera_jpeg_callback)(jpeg_event_t status);

} // extern "C"




struct preview_size_type {
    int width;
    int height;
};

static preview_size_type preview_sizes[] = {
    { 800, 480 }, // WVGA
    { 640, 480 }, // VGA
    { 480, 320 }, // HVGA
    { 384, 288 },
    { 352, 288 }, // CIF
    { 320, 240 }, // QVGA
    { 240, 160 }, // SQVGA
    { 176, 144 }, // QCIF
};

static int attr_lookup(const struct str_map *const arr, const char *name)
{
    if (name) {
        const struct str_map *trav = arr;
        while (trav->desc) {
            if (!strcmp(trav->desc, name))
                return trav->val;
            trav++;
        }
    }
    return NOT_FOUND;
}

#define INIT_VALUES_FOR(parm) do {                               \
    if (!parm##_values) {                                        \
        parm##_values = (char *)malloc(sizeof(parm)/             \
                                       sizeof(parm[0])*30);      \
        char *ptr = parm##_values;                               \
        const str_map *trav;                                     \
        for (trav = parm; trav->desc; trav++) {                  \
            int len = strlen(trav->desc);                        \
            strcpy(ptr, trav->desc);                             \
            ptr += len;                                          \
            *ptr++ = ',';                                        \
        }                                                        \
        *--ptr = 0;                                              \
    }                                                            \
} while(0)

// from aeecamera.h
static const str_map whitebalance[] = {
    { "auto",         CAMERA_WB_AUTO },
    { "incandescent", CAMERA_WB_INCANDESCENT },
    { "fluorescent",   CAMERA_WB_FLUORESCENT },
    { "daylight",     CAMERA_WB_DAYLIGHT },
    { "cloudy-daylight",       CAMERA_WB_CLOUDY_DAYLIGHT },
    { "twilight",     CAMERA_WB_TWILIGHT },
    { "shade",        CAMERA_WB_SHADE },
    { "horizon",        CAMERA_WB_HORIZON },    
    { NULL, 0 }
};
static char *whitebalance_values;

// from camera_effect_t
static const str_map effect[] = {
    { "none",       CAMERA_EFFECT_OFF },  /* This list must match aeecamera.h */
    { "mono",       CAMERA_EFFECT_MONO },
    { "negative",   CAMERA_EFFECT_NEGATIVE },
    { "solarize",   CAMERA_EFFECT_SOLARIZE },
    { "sepia",      CAMERA_EFFECT_SEPIA },
    { "aqua",       CAMERA_EFFECT_AQUA },
    { "blue-tint",  CAMERA_EFFECT_BLUE },
    { "green-tint", CAMERA_EFFECT_GREEN },
    { "red-tint",   CAMERA_EFFECT_RED },
    { "pink-tint",  CAMERA_EFFECT_PINK },
    { "yellow-tint",CAMERA_EFFECT_YELLOW },
    { "purple-tint",CAMERA_EFFECT_PURPLE },
    { "antique",    CAMERA_EFFECT_ANTIQUE },
    { "solarize2",  CAMERA_EFFECT_SOLARIZE2 },
    { "solarize3",  CAMERA_EFFECT_SOLARIZE3 },
    { "solarize4",  CAMERA_EFFECT_SOLARIZE4 },
    { "emboss",     CAMERA_EFFECT_EMBOSS },
    { "outline",    CAMERA_EFFECT_OUTLINE },
    { NULL, 0 }
};
static char *effect_values;

static const str_map iso[] = {
    { "auto",	M4MO_ISO_AUTO },
    { "50",	M4MO_ISO_50 },
    { "100",	M4MO_ISO_100 },
    { "200",	M4MO_ISO_200 },
    { "400",	M4MO_ISO_400 },
    { "800",	M4MO_ISO_800 },
    { "1000",	M4MO_ISO_1000 },
    { NULL, 0 }
} ;
static char *iso_values ;

static const str_map focusmode[] = {
    { "auto",	M4MO_AF_NORMAL },
    { "normal",	M4MO_AF_NORMAL },
    { "macro",	M4MO_AF_MACRO },
    { NULL, 0 }
} ;
static char *focusmode_values ;

static const str_map autoexposure[] = {
    { "meter-average",	M4MO_PHOTOMETRY_AVERAGE },
    { "meter-center",	M4MO_PHOTOMETRY_CENTER },
    { "meter-spot",	M4MO_PHOTOMETRY_SPOT },
    { NULL, 0 }
} ;
static char *autoexposure_values ;

static const str_map contrast[] = {
    { "0.0",	M4MO_CONTRAST_MINUS_2 },
    { "2.5",	M4MO_CONTRAST_MINUS_1 },
    { "5.0",	M4MO_CONTRAST_DEFAULT },
    { "7.5",	M4MO_CONTRAST_PLUS_1 },
    { "10.0",	M4MO_CONTRAST_PLUS_2 },
    { NULL, 0 }
} ;
static char *contrast_values ;

static const str_map saturation[] = {
    { "0.0",	M4MO_SATURATION_MINUS_2 },
    { "2.5",	M4MO_SATURATION_MINUS_1 },
    { "5.0",	M4MO_SATURATION_DEFAULT },
    { "7.5",	M4MO_SATURATION_PLUS_1 },
    { "10.0",	M4MO_SATURATION_PLUS_2 },
    { NULL, 0 }
} ;
static char *saturation_values ;

static const str_map sharpness[] = {
    { "0.0",	M4MO_SHARPNESS_MINUS_2 },
    { "5.0",	M4MO_SHARPNESS_MINUS_1 },
    { "10.0",	M4MO_SHARPNESS_DEFAULT },
    { "15.0",	M4MO_SHARPNESS_PLUS_1 },
    { "20.0",	M4MO_SHARPNESS_PLUS_2 },
    { NULL, 0 }
} ;
static char *sharpness_values ;

static const str_map exposure[] = {
    { "-8",	M4MO_EV_MINUS_4 },
    { "-6",	M4MO_EV_MINUS_3 },  
    { "-4",	M4MO_EV_MINUS_2 },
    { "-2",	M4MO_EV_MINUS_1 },
    { "0",	M4MO_EV_DEFAULT },
    { "2",	M4MO_EV_PLUS_1 },
    { "4",	M4MO_EV_PLUS_2 },
    { "6",	M4MO_EV_PLUS_3 },
    { "8",	M4MO_EV_PLUS_4 },
    { NULL, 0 }
} ;
static char *exposure_values ;
//4 2 0 -2 -4

// from qcamera/common/camera.h
static const str_map antibanding[] = {
    { "off",  CAMERA_ANTIBANDING_OFF },
    { NULL, 0 }
};
static char *antibanding_values;


static const str_map wdr[] = {
    { "off",  M4MO_WDR_OFF },
    { "on",  M4MO_WDR_ON },
    { NULL, 0 }
};
static char *wdr_values;

static const str_map isc[] = {
    { "off",  M4MO_ISC_STILL_OFF },
    { "on",  M4MO_ISC_STILL_ON },
    { "auto",  M4MO_ISC_STILL_AUTO },    
    { "movie-on",  M4MO_ISC_MOVIE_ON },   
    { NULL, 0 }
};
static char *isc_values;

static const str_map facedetection[] = {
    { "off",  M4MO_FACE_DETECTION_OFF },
    { "on",  M4MO_FACE_DETECTION_ON },
    { NULL, 0 }
};
static char *facedetection_values;

static const str_map aewb[] = {
    { "ae-lock-awb-lock",  M4MO_AE_LOCK_AWB_LOCK },
    { "ae-lock-awb-unlock",  M4MO_AE_LOCK_AWB_UNLOCK },
    { "ae-unlock-awb-lock",  M4MO_AE_UNLOCK_AWB_LOCK },    
    { "ae-unlock-awb-unlock",  M4MO_AE_UNLOCK_AWB_UNLOCK },   
    { NULL, 0 }
};
static char *aewb_values; 

static const str_map scenemode[] = {
    { "auto",  		M4MO_SCENE_AUTO },
    { "action",  	M4MO_SCENE_SPORTS },
    { "portrait",  	M4MO_SCENE_PORTRAIT },
    { "landscape",  	M4MO_SCENE_LANDSCAPE },
    { "night",  	M4MO_SCENE_NIGHT },
    { "beach",  	M4MO_SCENE_BEACH_SNOW },
    { "sunset",  	M4MO_SCENE_SUNSET },
    { "fireworks",  	M4MO_SCENE_FIREWORK },
    { NULL, 0 }
};
static char *scenemode_values;
        
//M4MO_SHOT_5M_SIZE ; M4MO_SHOT_3M_SIZE ; M4MO_SHOT_2M_SIZE ; M4MO_SHOT_1024_768_SIZE
//2560x1920,2048x1536,1600x1200,1024x768
static const str_map picturesize[] = {
    { "2560x1920",  	M4MO_SHOT_5M_SIZE },
    { "2048x1536",  	M4MO_SHOT_3M_SIZE },
    { "1600x1200",  	M4MO_SHOT_2M_SIZE },
    { "1024x768",  	M4MO_SHOT_1024_768_SIZE },
    { NULL, 0 }
};
static char *picturesize_values;
        
        
// round to the next power of two
static inline unsigned clp2(unsigned x)
{
    x = x - 1;
    x = x | (x >> 1);
    x = x | (x >> 2);
    x = x | (x >> 4);
    x = x | (x >> 8);
    x = x | (x >>16);
    return x + 1;
}

     static void dump_to_file(const char *fname,
                              uint8_t *buf, uint32_t size) ;

namespace android {

static Mutex singleton_lock;
static bool singleton_releasing;
static Condition singleton_wait;

static void receive_camframe_callback(struct msm_frame_t *frame);
static void receive_jpeg_fragment_callback(uint8_t *buff_ptr, uint32_t buff_size);
static void receive_jpeg_callback(jpeg_event_t status);
//static void receive_shutter_callback();

int cam_conf_sync[2];

static int camerafd;
pthread_t w_thread;

void *opencamerafd(void *data) {
	camerafd = open(MSM_CAMERA_CONTROL, O_RDWR);
	return NULL;
}

QualcommCameraHardware::QualcommCameraHardware()
    : mParameters(),
      mPreviewHeight(-1),
      mPreviewWidth(-1),
      mRawHeight(-1),
      mRawWidth(-1),
      mCameraRunning(false),
      mPreviewInitialized(false),
      mRawInitialized(false),
      mFrameThreadRunning(false),
      mSnapshotThreadRunning(false),
      mReleasedRecordingFrame(false),
      mNotifyCb(0),
      mDataCb(0),
      mDataCbTimestamp(0),
      mCallbackCookie(0),
      mMsgEnabled(0),
      mPreviewFrameSize(0),
      mRawSize(0),
      mCameraControlFd(-1),
      mAutoFocusThreadRunning(false),
      mAutoFocusFd(-1),
      mInPreviewCallback(false),
      mCameraRecording(false),
      mVideoFlashRunning(false)
{
    if((pthread_create(&w_thread, NULL, opencamerafd, NULL ))!=0){
    	LOGE("Camera open thread creation failed") ;
    }
    memset(&mDimension, 0, sizeof(mDimension));
    memset(&mCrop, 0, sizeof(mCrop));
    LOGD("constructor EX");
}

void QualcommCameraHardware::initDefaultParameters()
{
    CameraParameters p;

    LOGD("initDefaultParameters E");

    preview_size_type *ps = &preview_sizes[DEFAULT_PREVIEW_SETTING];
    p.setPreviewSize(ps->width, ps->height);
    p.setPreviewFrameRate(30);
    p.setPreviewFormat("yuv420sp"); // informative
    p.setPictureFormat("jpeg"); // informative

    p.set("jpeg-quality", "100"); // maximum quality
    p.set("jpeg-thumbnail-width", THUMBNAIL_WIDTH_STR); // informative
    p.set("jpeg-thumbnail-height", THUMBNAIL_HEIGHT_STR); // informative
    p.set("jpeg-thumbnail-quality", "85");

    p.setPictureSize(DEFAULT_PICTURE_WIDTH, DEFAULT_PICTURE_HEIGHT);
    p.set("antibanding", "off");
    p.set("effect", "none");
    p.set("whitebalance", "auto");
    p.set("flash-mode", "auto") ;
    p.set("iso", "auto");
    p.set("focus-mode", "normal") ;
    p.set("meter-mode", "meter-center") ;
#if 0
    p.set("gps-timestamp", "1199145600"); // Jan 1, 2008, 00:00:00
    p.set("gps-latitude", "37.736071"); // A little house in San Francisco
    p.set("gps-longitude", "-122.441983");
    p.set("gps-altitude", "21"); // meters
#endif

    // This will happen only one in the lifetime of the mediaserver process.
    // We do not free the _values arrays when we destroy the camera object.
    INIT_VALUES_FOR(antibanding);
    INIT_VALUES_FOR(effect);
    INIT_VALUES_FOR(whitebalance);
    INIT_VALUES_FOR(iso);
    INIT_VALUES_FOR(focusmode);
    INIT_VALUES_FOR(contrast);
    INIT_VALUES_FOR(saturation);
    INIT_VALUES_FOR(sharpness);
    INIT_VALUES_FOR(exposure);
    INIT_VALUES_FOR(autoexposure);
    INIT_VALUES_FOR(facedetection);
    INIT_VALUES_FOR(wdr);
    INIT_VALUES_FOR(isc);
    INIT_VALUES_FOR(aewb);
    INIT_VALUES_FOR(scenemode) ;
    INIT_VALUES_FOR(picturesize) ;

    
    p.set("antibanding-values", antibanding_values);
    p.set("effect-values", effect_values);
    p.set("whitebalance-values", whitebalance_values);
    p.set("iso-values", iso_values ) ;
    p.set("focus-mode-values", focusmode_values ) ;    
    p.set("meter-mode-values", autoexposure_values ) ;
    
    p.set("wide-dynamic-range-values", wdr_values ) ;
    p.set("image-stabilization-control-values", isc_values ) ;
    p.set("face-detection-values", facedetection_values ) ;
    p.set("aewb-lock-values", aewb_values ) ; 
 
    p.set("scene-mode-values", scenemode_values ) ;
    
    p.set("wide-dynamic-range", "off") ;
    p.set("image-stabilization-control", "off") ;
    p.set("face-detection", "off") ;
    p.set("aewb-lock", "ae-lock-awb-lock") ;
 
    p.set("scene-mode", "auto" ) ;
    
    p.set("picture-size-values", "2560x1920,2048x1536,1600x1200,1024x768");
    p.set("preview-size-values", "384x288");
    p.set("flash-mode-values", "off,auto,on") ;

    
    p.set("zoom-ratios", "100,114,131,151,174,200,214,231,251,274,300") ;
    p.set("zoom-supported", "true") ;
    p.set("zoom", "0" ) ;
    p.set("max-zoom", "10" ) ;
    
    p.set("contrast-def","3") ;
    p.set("contrast-max","6") ;
    p.set("contrast-min","0") ;
    p.set("contrast", "3") ;
    

    p.set("exposure-compensation-step", "1") ;
    p.set("exposure-compensation", "0") ;
    p.set("min-exposure-compensation", "-4") ;
    p.set("max-exposure-compensation", "4") ;
	  
    p.set("saturation-def", "3") ;
    p.set("saturation-max", "6") ;
    p.set("saturation-min", "0") ;
    p.set("saturation", "3") ;
	  
/*    p.set("sharpness-def", "10") ;
    p.set("sharpness-max", "30") ;
    p.set("sharpness-min", "0") ;
    p.set("sharpness", "15.0") ;*/

        p.set("sharpness-def", "3") ;
        p.set("sharpness-max", "6") ;
        p.set("sharpness-min", "0") ;
        p.set("sharpness", "3") ;

    mIso = M4MO_ISO_AUTO ;
    mEffect = 1 ;
    mWhiteBalance = 1;
    mFocusMode = 1 ;
    mZoom = 0 ;
    mContrast = 3 ;
    mSaturation = 3 ;
    mSharpness = 3 ;
    mExposure = 5 ;
    mAutoExposure = 2 ;
    mAEWB = 1 ;
    mWideDynamicRange = 1 ;
    mImageStabilizationControl = 1 ;
    mFaceDetection = 1 ;
    mSceneMode = 1 ;
    
    if (setParameters(p) != NO_ERROR) {
        LOGE("Failed to set default parameters?!");
    }

    LOGD("initDefaultParameters X");
}

void QualcommCameraHardware::setCallbacks(notify_callback notify_cb,
                                      data_callback data_cb,
                                      data_callback_timestamp data_cb_timestamp,
                                      void* user)
{
    Mutex::Autolock lock(mLock);
    mNotifyCb = notify_cb;
    mDataCb = data_cb;
    mDataCbTimestamp = data_cb_timestamp;
    mCallbackCookie = user;
}

void QualcommCameraHardware::enableMsgType(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    LOGD("enableMsgType( %d )", msgType ) ;
    mMsgEnabled |= msgType;
}

void QualcommCameraHardware::disableMsgType(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    mMsgEnabled &= ~msgType;
}

bool QualcommCameraHardware::msgTypeEnabled(int32_t msgType)
{
    Mutex::Autolock lock(mLock);
    return (mMsgEnabled & msgType);
}


#define ROUND_TO_PAGE(x)  (((x)+0xfff)&~0xfff)

void QualcommCameraHardware::startCamera()
{
   unsigned char sync_value;
    LOGD("startCamera E");
    libmmcamera = ::dlopen("libmmcamera.so", RTLD_NOW);
    LOGD("loading liboemcamera at %p", libmmcamera);
    if (!libmmcamera) {
        LOGE("FATAL ERROR: could not dlopen liboemcamera.so: %s", dlerror());
        return;
    }

    libmmcamera_target = ::dlopen("libmm-qcamera-tgt.so", RTLD_NOW) ;
    LOGD("loading libmm-qcamera-tgt.so %p", libmmcamera_target) ;
    if (!libmmcamera_target) {
    	LOGE("FATAL ERROR: could not dlopen libmm-qcamera-tgt.so: %s", dlerror());
    	return;
    }


    *(void **)&LINK_cam_frame =
        ::dlsym(libmmcamera, "cam_frame");
    *(void **)&LINK_camframe_terminate =
        ::dlsym(libmmcamera, "camframe_terminate");

    *(void **)&LINK_jpeg_encoder_init =
        ::dlsym(libmmcamera, "jpeg_encoder_init");

    *(void **)&LINK_jpeg_encoder_encode =
        ::dlsym(libmmcamera, "jpeg_encoder_encode");

    *(void **)&LINK_jpeg_encoder_join =
        ::dlsym(libmmcamera, "jpeg_encoder_join");

    *(void **)&LINK_mmcamera_camframe_callback =
        ::dlsym(libmmcamera, "camframe_callback");

    *LINK_mmcamera_camframe_callback = receive_camframe_callback;

    *(void **)&LINK_mmcamera_jpegfragment_callback =
        ::dlsym(libmmcamera, "jpegfragment_callback");

    *LINK_mmcamera_jpegfragment_callback = receive_jpeg_fragment_callback;

    *(void **)&LINK_mmcamera_jpeg_callback =
        ::dlsym(libmmcamera, "jpeg_callback");

    *LINK_mmcamera_jpeg_callback = receive_jpeg_callback;

    *(void**)&LINK_jpeg_encoder_setMainImageQuality =
        ::dlsym(libmmcamera, "jpeg_encoder_setMainImageQuality");

    *(void**)&LINK_jpeg_encoder_setThumbnailQuality =
        ::dlsym(libmmcamera, "jpeg_encoder_setThumbnailQuality");

    *(void**)&LINK_jpeg_encoder_setRotation =
        ::dlsym(libmmcamera, "jpeg_encoder_setRotation");

    *(void**)&LINK_jpeg_encoder_setLocation =
        ::dlsym(libmmcamera, "jpeg_encoder_setLocation");

    *(void **)&LINK_cam_conf =
        ::dlsym(libmmcamera_target, "cam_conf");

    /* The control thread is in libcamera itself. */
//    mCameraControlFd = open(MSM_CAMERA_CONTROL, O_RDWR);
LOGD("join open thread") ;
    if( pthread_join(w_thread, NULL) != 0 ) {
    	LOGE("Camera open thread exit failed") ;
	return ;
    }
    mCameraControlFd = camerafd ;

LOGD("before LINK_jpeg_encoder_init") ;    
    if (!LINK_jpeg_encoder_init()) {
      LOGE("jpeg_encoding_init failed.\n") ;
    }
LOGD("LINK_jpeg_encoder_init done") ;        


    /*if (pipe(cam_conf_sync) < 0) {
             LOGE("cam_conf_sync pipe create failed");
             return;
    }*/
LOGD("pipe done") ;        


    if (mCameraControlFd < 0) {
        LOGE("startCamera X: %s open failed: %s!",
             MSM_CAMERA_CONTROL,
             strerror(errno));
//         return;d
    }


LOGD("before cam_conf thread") ;        
    if( (pthread_create(&mCamConfigThread, NULL,
                   LINK_cam_conf, NULL )) != 0 ) { //(void*)&(cam_conf_sync[1])
      LOGE("Config thread creation failed !") ;
    } 
    
    
//  usleep(500*1000); // use sleep value found in old qualcomm code
       
    m4mo_get_firmware_version() ;

    /*close(cam_conf_sync[1]);

    if (read(cam_conf_sync[0], &sync_value, sizeof(sync_value)) < 0) {
             LOGE("thread sync failed");
             close(cam_conf_sync[0]);
             return ;
         }
         close(cam_conf_sync[0]);
         close(cam_conf_sync[1]);
         if (sync_value) {
		LOGE("error : sync_value is true") ;
                return;
	 }
*/
    LOGD("startCamera X");
}

status_t QualcommCameraHardware::dump(int fd,
                                      const Vector<String16>& args) const
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;

    // Dump internal primitives.
    result.append("QualcommCameraHardware::dump");
    snprintf(buffer, 255, "preview width(%d) x height (%d)\n",
             mPreviewWidth, mPreviewHeight);
    result.append(buffer);
    snprintf(buffer, 255, "raw width(%d) x height (%d)\n",
             mRawWidth, mRawHeight);
    result.append(buffer);
    snprintf(buffer, 255,
             "preview frame size(%d), raw size (%d), jpeg size (%d) "
             "and jpeg max size (%d)\n", mPreviewFrameSize, mRawSize,
             mJpegSize, mJpegMaxSize);
    result.append(buffer);
    write(fd, result.string(), result.size());

    // Dump internal objects.
    if (mPreviewHeap != 0) {
        mPreviewHeap->dump(fd, args);
    }
    if (mRawHeap != 0) {
        mRawHeap->dump(fd, args);
    }
    if (mJpegHeap != 0) {
        mJpegHeap->dump(fd, args);
    }
    mParameters.dump(fd, args);
    return NO_ERROR;
}

static bool native_start_preview(int camfd)
{
  int ioctlRetVal = 1 ;
  
    struct msm_ctrl_cmd_t ctrlCmd;

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.type       = CAMERA_START_PREVIEW;
    ctrlCmd.length     = 0;
    ctrlCmd.value = NULL ;
//    ctrlCmd.resp_fd    = camfd; // FIXME: this will be put in by the kernel
    
//sensor_mode_t mode
//cfg_data.cfgtype == CFG_SET_MODE
//cfg_data.mode == SENSOR_PREVIEW_MODE
//MSM_CAM_IOCTL_SENSOR_IO_CFG
// 

    ioctlRetVal = ioctl(camfd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd) ;
    if ( ioctlRetVal < 0) {
        LOGE("native_start_preview: MSM_CAM_IOCTL_CTRL_COMMAND fd %d error %s",
             camfd,
             strerror(errno));
	LOGE("ioctlRetVal %d", ioctlRetVal ) ;
        return false;
    }
    LOGD("native_start_preview status after ioctl == %d" ,ctrlCmd.status ) ;


    return true;
}

static bool native_get_picture (int camfd, common_crop_t *crop)
{
    struct msm_ctrl_cmd_t ctrlCmd;

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.length     = sizeof(common_crop_t);
    ctrlCmd.value      = crop;

    if(ioctl(camfd, MSM_CAM_IOCTL_GET_PICTURE, &ctrlCmd) < 0) {
        LOGE("native_get_picture: MSM_CAM_IOCTL_GET_PICTURE fd %d error %s",
             camfd,
             strerror(errno));
        return false;
    }

    LOGD("crop: in1_w %d", crop->in1_w);
    LOGD("crop: in1_h %d", crop->in1_h);
    LOGD("crop: out1_w %d", crop->out1_w);
    LOGD("crop: out1_h %d", crop->out1_h);

    LOGD("crop: in2_w %d", crop->in2_w);
    LOGD("crop: in2_h %d", crop->in2_h);
    LOGD("crop: out2_w %d", crop->out2_w);
    LOGD("crop: out2_h %d", crop->out2_h);

    LOGD("crop: update %d", crop->update_flag);

    LOGD("native_get_picture status after ioctl == %d" ,ctrlCmd.status ) ;
    
    return true;
}

static bool native_stop_preview(int camfd)
{
    struct msm_ctrl_cmd_t ctrlCmd;
    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.type       = CAMERA_STOP_PREVIEW;
    ctrlCmd.length     = 0;
//    ctrlCmd.resp_fd    = camfd; // FIXME: this will be put in by the kernel

    if(ioctl(camfd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd) < 0) {
        LOGE("native_stop_preview: ioctl fd %d error %s",
             camfd,
             strerror(errno));
        return false;
    }
    LOGD("native_stop_preview status after ioctl == %d" ,ctrlCmd.status ) ;
    return true;
}

static bool native_start_snapshot(int camfd)
{
    struct msm_ctrl_cmd_t ctrlCmd;

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.type       = CAMERA_START_SNAPSHOT;
    ctrlCmd.length     = 0;
//    ctrlCmd.resp_fd    = camfd; // FIXME: this will be put in by the kernel

    if(ioctl(camfd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd) < 0) {
        LOGE("native_start_snapshot: ioctl fd %d error %s",
             camfd,
             strerror(errno));
        return false;
    }
    LOGD("native_start_snapshot status after ioctl == %d" ,ctrlCmd.status ) ;
    return true;
}

static bool native_stop_snapshot (int camfd)
{
    struct msm_ctrl_cmd_t ctrlCmd;

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.type       = CAMERA_STOP_SNAPSHOT;
    ctrlCmd.length     = 0;
//    ctrlCmd.resp_fd    = camfd; // FIXME: this will be put in by the kernel

    if (ioctl(camfd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd) < 0) {
        LOGE("native_stop_snapshot: ioctl fd %d error %s",
             camfd,
             strerror(errno));
        return false;
    }
    LOGD("native_stop_snapshot status after ioctl == %d" ,ctrlCmd.status ) ;
    return true;
}


void *jpeg_encoder_thread( void *user ) 
{
  LOGD("jpeg_encoder_thread E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->runJpegEncodeThread(user);
    }
    else LOGW("not starting frame thread: the object went away!");
    LOGD("jpeg_encoder_thread X");
    return NULL;
}

bool QualcommCameraHardware::native_jpeg_encode(void)
{
    int jpeg_quality = mParameters.getInt("jpeg-quality");
    if (jpeg_quality >= 0) {
        LOGD("native_jpeg_encode, current jpeg main img quality =%d",
             jpeg_quality);
        if(!LINK_jpeg_encoder_setMainImageQuality(jpeg_quality)) {
            LOGE("native_jpeg_encode set jpeg-quality failed");
            return false;
        }
      LOGD("jpeg main img quality done ") ;
    }
//   
    int thumbnail_quality = mParameters.getInt("jpeg-thumbnail-quality");
    if (thumbnail_quality >= 0) {
        LOGD("native_jpeg_encode, current jpeg thumbnail quality =%d",
             thumbnail_quality);
      //  if(!LINK_jpeg_encoder_setThumbnailQuality(thumbnail_quality)) {
      //      LOGE("native_jpeg_encode set thumbnail-quality failed");
      //      return false;
      //  }
      LOGD("jpeg thumbnail-quality done ") ;        
    }
    int rotation = mParameters.getInt("rotation");
    if (rotation >= 0) {
        LOGD("native_jpeg_encode, rotation = %d", rotation);
    }
    char jpegFileName[256] = {0};
    static int snapshotCntr = 0;

    mDimension.filler7 = 2560 ;
    mDimension.filler8 = 1920 ;

    
    
	  LOGD("picture_width %d, picture_height = %d, display_width = %d, display_height = %d, filler = %d, filler2 = %d, ui_thumbnail_height = %d , ui_thumbnail_width = %d, filler3 = %d, filler4 = %d, filler5 = %d, filler6 = %d, filler7 = %d, filler8 = %d\n" , 
		 mDimension.picture_width,mDimension.picture_height,
		 mDimension.display_width,mDimension.display_height, 
	         mDimension.filler, mDimension.filler2, 
		mDimension.ui_thumbnail_height, mDimension.ui_thumbnail_width, 
	       mDimension.filler3, mDimension.filler4, mDimension.filler5, mDimension.filler6, 
	       mDimension.filler7, mDimension.filler8 );
    
	       
            pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	/*struct sigaction actions ;
	memset( &actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask); 
	actions.sa_flags = 0 ;
	actions.sa_handler = exit_camframe_handler ;
	sigaction( SIGUSR1, &actions, NULL ) ;*/
	
	pthread_t jpegThread ;
	
        int ret = !pthread_create(&jpegThread,
                                              &attr, //NULL, 
                                              jpeg_encoder_thread,
                                              NULL);
					      
    /*sprintf(jpegFileName, "snapshot_%d.jpg", ++snapshotCntr);
    if ( !LINK_jpeg_encoder_encode(jpegFileName, &mDimension,  
                             (uint8_t *)mThumbnailHeap->mHeap->base(), mThumbnailHeap->mHeap->getHeapID(),
			      (uint8_t *)mRawHeap->mHeap->base(), mRawHeap->mHeap->getHeapID(), 
				   &mCrop)) {
       LOGV("native_jpeg_encode:%d@%s: jpeg_encoder_encode failed.\n", __LINE__, __FILE__);
     return false;
   }*/
   
   /*
    if (!LINK_jpeg_encoder_encode(&mDimension,
                                  mThumbnailHeap->mHeap->getHeapID(),
                                  mRawHeap->mHeap->getHeapID(),
				  (uint8_t *)mThumbnailHeap->mHeap->base(),
                                  (uint8_t *)mRawHeap->mHeap->base(),
                                  &mCrop)) {
        LOGE("native_jpeg_encode: jpeg_encoder_encode failed.");
        return false;
    }*/
    return true;
}

bool QualcommCameraHardware::native_set_dimension(cam_ctrl_dimension_t *value)
{
    return native_set_parm(CAMERA_SET_PARM_DIMENSION,
                           sizeof(cam_ctrl_dimension_t), value);
}

bool QualcommCameraHardware::native_set_parm(
    cam_ctrl_type type, uint16_t length, void *value)
{
    int rc = true;
    struct msm_ctrl_cmd_t ctrlCmd;

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.type       = (uint16_t)type;
    ctrlCmd.length     = length;
    // FIXME: this will be put in by the kernel
//    ctrlCmd.resp_fd    = mCameraControlFd;
    ctrlCmd.value = value;

    LOGD("native_set_parm. camfd=%d, type=%d, length=%d",
         mCameraControlFd, type, length);
    rc = ioctl(mCameraControlFd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd);
    if(rc < 0 || ctrlCmd.status != CAM_CTRL_SUCCESS) {
        LOGE("ioctl error. camfd=%d, type=%d, length=%d, rc=%d, ctrlCmd.status=%d, %s",
             mCameraControlFd, type, length, rc, ctrlCmd.status, strerror(errno));
        return false;
    }
    return true;
}

void QualcommCameraHardware::runFrameThread(void *data)
{
    LOGD("runFrameThread E");

    int cnt;

    // We need to maintain a reference to liboemcamera.so for the duration of the
    // frame thread, because we do not know when it will exit relative to the
    // lifetime of this object.  We do not want to dlclose() liboemcamera while
    // LINK_cam_frame is still running.
    void *libhandle = ::dlopen("libmmcamera.so", RTLD_NOW);
    LOGD("FRAME: loading libmmcamera at %p", libhandle);
    if (!libhandle) {
        LOGE("FATAL ERROR: could not dlopen liboemcamera.so: %s", dlerror());
    }
    if (libhandle)
    {
      LOGD("Before LINK_cam_frame") ;
        LINK_cam_frame(data);
    LOGD("After LINK_cam_frame") ;	
    }


    if (libhandle) {
        ::dlclose(libhandle);
        LOGD("FRAME: dlclose(libmmcamera)");
    }

    mFrameThreadWaitLock.lock();
    mFrameThreadRunning = false;
    mFrameThreadWait.signal();
    mFrameThreadWaitLock.unlock();

    LOGD("runFrameThread X");
}

void QualcommCameraHardware::runJpegEncodeThread(void *data)
{
      unsigned char *buffer ;
      
//     readFromMemory( (unsigned char *)mRawHeap->mHeap->base(), 2097152, buffer ) ;
//      writeToMemory( buffer, 2560, 1920, (unsigned char *)mJpegHeap->mHeap->base(), (int *)&mJpegSize ) ;
      
      
    int rotation = mParameters.getInt("rotation");
    LOGD("native_jpeg_encode, rotation = %d", rotation);
    
         bool encode_location = true;
	camera_position_type pt;

#define PARSE_LOCATION(what,type,fmt,desc) do { \
pt.what = 0; \
const char *what##_str = mParameters.get("gps-"#what); \
LOGD("GPS PARM %s --> [%s]", "gps-"#what, what##_str); \
if (what##_str) { \
type what = 0; \
if (sscanf(what##_str, fmt, &what) == 1) \
pt.what = what; \
else { \
LOGE("GPS " #what " %s could not" \
" be parsed as a " #desc, what##_str); \
encode_location = false; \
} \
} \
else { \
LOGD("GPS " #what " not specified: " \
"defaulting to zero in EXIF header."); \
encode_location = false; \
} \
} while(0)

    PARSE_LOCATION(timestamp, long, "%ld", "long");
    if (!pt.timestamp) pt.timestamp = time(NULL);
    PARSE_LOCATION(altitude, short, "%hd", "short");
    PARSE_LOCATION(latitude, double, "%lf", "double float");
    PARSE_LOCATION(longitude, double, "%lf", "double float");

#undef PARSE_LOCATION

    if (encode_location) {
        LOGD("setting image location ALT %d LAT %lf LON %lf",
             pt.altitude, pt.latitude, pt.longitude);
    }
    else {
      LOGV("not setting image location");
      
    }
    
      LOGD("mJpegSize %d" , mJpegSize ) ;
      
 camera_position_type *npt = &pt ;
 if( ! encode_location ) {
   npt = NULL ;
 }
    writeExif( mRawHeap->mHeap->base(), mJpegHeap->mHeap->base(), mJpegSize, &mJpegSize, rotation , npt ) ;

      receiveJpegPicture();
}

void *frame_thread(void *user)
{
    LOGD("frame_thread E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->runFrameThread(user);
    }
    else LOGW("not starting frame thread: the object went away!");
    LOGD("frame_thread X");
    return NULL;
}


static bool register_buf(int camfd,
                         int size,
                         int pmempreviewfd,
                         uint32_t offset,
                         uint8_t *buf,
                         msm_pmem_t mem_type,
			 bool active,
                         bool register_buffer = true);
bool QualcommCameraHardware::initPreview()
{
    // See comments in deinitPreview() for why we have to wait for the frame
    // thread here, and why we can't use pthread_join().
    LOGI("initPreview E: preview size=%dx%d", mPreviewWidth, mPreviewHeight);
    LOGD("running custom built libcamera.so for samsung galaxy by npinot based on NCommander for dream");
    mFrameThreadWaitLock.lock();
    while (mFrameThreadRunning) {
        LOGD("initPreview: waiting for old frame thread to complete.");
        mFrameThreadWait.wait(mFrameThreadWaitLock);
        LOGD("initPreview: old frame thread completed.");
    }
    mFrameThreadWaitLock.unlock();

    mSnapshotThreadWaitLock.lock();
    while (mSnapshotThreadRunning) {
        LOGD("initPreview: waiting for old snapshot thread to complete.");
        mSnapshotThreadWait.wait(mSnapshotThreadWaitLock);
        LOGD("initPreview: old snapshot thread completed.");
    }
    mSnapshotThreadWaitLock.unlock();



    mDimension.picture_width  = DEFAULT_PICTURE_WIDTH;
    mDimension.picture_height = DEFAULT_PICTURE_HEIGHT;

    bool ret = native_set_dimension(&mDimension);

    if (ret) {
      
          int cnt = 0;
	mPreviewFrameSize = mPreviewWidth * mPreviewHeight * 3/2;
	LOGD("mPreviewFrameSize = %d", mPreviewFrameSize ) ;
	mPreviewHeap = new PreviewPmemPool(
				    mCameraControlFd,
				    kRawFrameHeaderSize + mPreviewFrameSize,
				    kPreviewBufferCount,
				    mPreviewFrameSize,kRawFrameHeaderSize,
				    "preview");

	if (!mPreviewHeap->initialized()) {
	    mPreviewHeap.clear();
	    LOGE("initPreview X: could not initialize preview heap.");
	    return false;
	}
    

         frame_size= (clp2(mDimension.display_width * mDimension.display_height *3/2));
           unsigned char activeBuffer;
                 LOGI("hal display_width = %d height = %d frame_size = %d\n",
                          (int)mDimension.display_width, (int)mDimension.display_height, frame_size);
			  
	//int frame_size= (clp2(mDimension.display_width * mDimension.display_height *3/2));
        for (cnt = 0; cnt < kPreviewBufferCount; cnt++) {
               frames[cnt].fd = mPreviewHeap->mHeapnew[cnt]->heapID();
               frames[cnt].buffer = (unsigned long)mPreviewHeap->mHeapnew[cnt]->base();
               LOGE("hal_mmap #%d start = %x end = %x", (int)cnt, (int)frames[cnt].buffer,
                    (int)(frames[cnt].buffer + frame_size - 1));

               frames[cnt].y_off = 0;
               frames[cnt].cbcr_off = mDimension.display_width * mDimension.display_height;

		if (frames[cnt].buffer == 0) {
			LOGV("main: malloc failed!\n");
			return 0;
		  }
	      frames[cnt].path = MSM_FRAME_ENC;
	 
	      register_buf(mCameraControlFd,
                         mDimension.display_width * mDimension.display_height * 3/2,
                         frames[cnt].fd ,
                         0,
                         (uint8_t *)mPreviewHeap->mHeapnew[cnt]->base() ,			 
                         MSM_PMEM_OUTPUT2, cnt == ( kPreviewBufferCount - 1 ) ? false : true, true );
        }

        mFrameThreadWaitLock.lock();
        pthread_attr_t attr;
        pthread_attr_init(&attr);
        pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	
	/*struct sigaction actions ;
	memset( &actions, 0, sizeof(actions));
	sigemptyset(&actions.sa_mask); 
	actions.sa_flags = 0 ;
	actions.sa_handler = exit_camframe_handler ;
	sigaction( SIGUSR1, &actions, NULL ) ;*/
	
        mFrameThreadRunning = !pthread_create(&mFrameThread,
                                              &attr, //NULL, 
                                              frame_thread,
                                              &frames[kPreviewBufferCount-1]);
        ret = mFrameThreadRunning;
        mFrameThreadWaitLock.unlock();
    }

    LOGD("initPreview X: %d", ret);
    return ret;
}


void QualcommCameraHardware::deinitPreview(void)
{
    LOGI("deinitPreview E");

    // When we call deinitPreview(), we signal to the frame thread that it
    // needs to exit, but we DO NOT WAIT for it to complete here.  The problem
    // is that deinitPreview is sometimes called from the frame-thread's
    // callback, when the refcount on the Camera client reaches zero.  If we
    // called pthread_join(), we would deadlock.  So, we just call
    // LINK_camframe_terminate() in deinitPreview(), which makes sure that
    // after the preview callback returns, the camframe thread will exit.  We
    // could call pthread_join() in initPreview() to join the last frame
    // thread.  However, we would also have to call pthread_join() in release
    // as well, shortly before we destoy the object; this would cause the same
    // deadlock, since release(), like deinitPreview(), may also be called from
    // the frame-thread's callback.  This we have to make the frame thread
    // detached, and use a separate mechanism to wait for it to complete.

//    if (LINK_camframe_terminate() < 0)
 //       LOGE("failed to stop the camframe thread: %s",
//             strerror(errno));

	  
    int rc = LINK_camframe_terminate() ;
  
    LOGD("camframe terminate rc = %d", rc ) ;

LOGD("Unregister buffers") ;
    for (int cnt = 0; cnt < kPreviewBufferCount; cnt++) {
	register_buf(mCameraControlFd,
                         mDimension.display_width * mDimension.display_height * 3/2,
                         mPreviewHeap->mHeapnew[cnt]->heapID() ,
                         0,
                         (uint8_t *)mPreviewHeap->mHeapnew[cnt]->base() ,			 
                         MSM_PMEM_OUTPUT2, cnt == ( kPreviewBufferCount - 1 ) ? false : true, false );
			 
    }
    
        mPreviewHeap.clear();

/*

*/			 
    LOGI("deinitPreview X");
}

bool QualcommCameraHardware::initRaw(bool initJpegHeap)
{
  mRawWidth = 2048 ;
  mRawHeight = 930 ;
  LOGD("initRaw E: picture size=%dx%d",
         mRawWidth, mRawHeight);


    mDimension.picture_width   = mRawWidth;
    mDimension.picture_height  = mRawHeight;
    mRawSize = mRawWidth * mRawHeight * 3 / 2;
    mJpegMaxSize = mRawWidth * mRawHeight * 3 / 2;

    mJpegMaxSize = 1904640 ;
    
    if(!native_set_dimension(&mDimension)) {
        LOGE("initRaw X: failed to set dimension");
        return false;
    }

    if (mJpegHeap != NULL) {
        LOGD("initRaw: clearing old mJpegHeap.");
        mJpegHeap.clear();
    }

    // Snapshot

    LOGD("initRaw: initializing mRawHeap.");
    mRawHeap =
        new PmemPool("/dev/pmem_adsp",
                     mCameraControlFd,
                     MSM_PMEM_MAINIMG,
                     mJpegMaxSize,
                     kRawBufferCount,
                     mRawSize,
                     0,
                     "snapshot camera");

    if (!mRawHeap->initialized()) {
        LOGE("initRaw X failed with pmem_camera, trying with pmem_adsp");
        mRawHeap =
            new PmemPool("/dev/pmem_adsp",
                         mCameraControlFd,
                         MSM_PMEM_MAINIMG,
                         mJpegMaxSize,
                         kRawBufferCount,
                         mRawSize,
                         0,
                         "snapshot camera" );
        if (!mRawHeap->initialized()) {
            mRawHeap.clear();
            LOGE("initRaw X: error initializing mRawHeap");
            return false;
        }
    }

        // Thumbnails

        mThumbnailHeap =
            new PmemPool("/dev/pmem_adsp",
                         mCameraControlFd,
                         MSM_PMEM_THUMBAIL,
                         262144, //THUMBNAIL_BUFFER_SIZE,
                         1,
                         262144, //THUMBNAIL_BUFFER_SIZE,
                         0,
                         "thumbnail");
       //  thumbnail_buf = hal_mmap(THUMBNAIL_BUFFER_SIZE, 
         //                                &pmemThumbnailfd);
			
        if (!mThumbnailHeap->initialized()) {
            mThumbnailHeap.clear();
            mRawHeap.clear();
            LOGE("initRaw X failed: error initializing mThumbnailHeap.");
            return false;
        }

	  
		  

/*
static bool register_buf(int camfd,
                         int size,
                         int pmempreviewfd,
                         uint32_t offset,
                         uint8_t *buf,
                         msm_pmem_t pmem_type,
			 bool active ,
                         bool register_buffer)
*/

    LOGD("do_mmap snapshot pbuf = %p, pmem_fd = %d",
         (uint8_t *)mRawHeap->mHeap->base(), mRawHeap->mHeap->getHeapID());

    // Jpeg

    if (initJpegHeap) {
        LOGD("initRaw: initializing mJpegHeap.");
        mJpegHeap =
            new AshmemPool(2097152,
                           kJpegBufferCount,
                           0, // we do not know how big the picture wil be
                           0,
                           "jpeg");

        if (!mJpegHeap->initialized()) {
            mJpegHeap.clear();
            mRawHeap.clear();
            LOGE("initRaw X failed: error initializing mJpegHeap.");
            return false;
        }


    }
    
    register_buf( mCameraControlFd, 
		  262144,mThumbnailHeap->mHeap->getHeapID(), 0, 
		  (uint8_t *)mThumbnailHeap->mHeap->base(), MSM_PMEM_THUMBAIL, true, true ) ;
//TODO why 2097152 and not mJpegMaxSize 
    register_buf( mCameraControlFd, 
		  2097152,mRawHeap->mHeap->getHeapID(), 0, 
		  (uint8_t *)mRawHeap->mHeap->base(), MSM_PMEM_RAW_MAINIMG, true, true ) ;	    
    mRawInitialized = true ;
    LOGD("initRaw X");
    return true;
}

void QualcommCameraHardware::deinitRaw()
{
    LOGD("deinitRaw E");
    register_buf( mCameraControlFd, 
		  262144,mThumbnailHeap->mHeap->getHeapID(), 0, 
		  (uint8_t *)mThumbnailHeap->mHeap->base(), MSM_PMEM_THUMBAIL, true, false ) ;
		  
    register_buf( mCameraControlFd, 
		  2097152,mRawHeap->mHeap->getHeapID(), 0, 
		  (uint8_t *)mRawHeap->mHeap->base(), MSM_PMEM_RAW_MAINIMG, true, false ) ;	
		  
    mThumbnailHeap.clear();
    mJpegHeap.clear();
    mRawHeap.clear();
    mRawInitialized = false ;
    LOGD("deinitRaw X");
}

void QualcommCameraHardware::release()
{
    LOGD("release E");
    Mutex::Autolock l(&mLock);

    if (libmmcamera == NULL) {
        LOGE("ERROR: multiple release!");
        return;
    }

    int cnt, rc;
    struct msm_ctrl_cmd_t ctrlCmd;

    if (mCameraRunning) {
      if(mMsgEnabled & CAMERA_MSG_VIDEO_FRAME) {
            mRecordFrameLock.lock();
            mReleasedRecordingFrame = true;
            mRecordWait.signal();
            mRecordFrameLock.unlock();
        }
        stopPreviewInternal();
    }

    LINK_jpeg_encoder_join();
    if( mRawInitialized ) {
      deinitRaw();
    }
    setLensToBasePosition();

    ctrlCmd.timeout_ms = 5000;
    ctrlCmd.length = 0;
    ctrlCmd.type = (uint16_t)CAMERA_EXIT;
//    ctrlCmd.resp_fd = mCameraControlFd; // FIXME: this will be put in by the kernel
    if (ioctl(mCameraControlFd, MSM_CAM_IOCTL_CTRL_COMMAND, &ctrlCmd) < 0)
        LOGE("ioctl CAMERA_EXIT fd %d error %s",
             mCameraControlFd, strerror(errno));
    rc = pthread_join(mCamConfigThread, NULL);
    if (rc)
        LOGE("config_thread exit failure: %s", strerror(errno));
    else
        LOGD("pthread_join succeeded on config_thread");

    close(mCameraControlFd);
    mCameraControlFd = -1;

    if (libmmcamera) {
        ::dlclose(libmmcamera);
        LOGD("dlclose(libmmcamera)");
        libmmcamera = NULL;
    }
    if (libmmcamera_target) {
    	::dlclose(libmmcamera_target);
	LOGD("dlclose(libmmcamera_target)");
	libmmcamera_target = NULL ;
    }

    Mutex::Autolock lock(&singleton_lock);
    singleton_releasing = true;

    LOGD("release X");
}

QualcommCameraHardware::~QualcommCameraHardware()
{
    LOGD("~QualcommCameraHardware E");
    Mutex::Autolock lock(&singleton_lock);
    singleton.clear();
    singleton_releasing = false;
    singleton_wait.signal();
    LOGD("~QualcommCameraHardware X");
}

sp<IMemoryHeap> QualcommCameraHardware::getRawHeap() const
{
    LOGD("getRawHeap");
    return mRawHeap != NULL ? mRawHeap->mHeap : NULL;
}

sp<IMemoryHeap> QualcommCameraHardware::getPreviewHeap(int32_t i) const
{
    LOGD("getPreviewHeap(%d)", i );
    return mPreviewHeap != NULL ? mPreviewHeap->mHeapnew[i] : NULL;
}

status_t QualcommCameraHardware::startPreviewInternal()
{
    if(mCameraRunning) {
        LOGD("startPreview X: preview already running.");
        return NO_ERROR;
    }

    if (!mPreviewInitialized) {
        mPreviewInitialized = initPreview();
        if (!mPreviewInitialized) {
            LOGE("startPreview X initPreview failed.  Not starting preview.");
            return UNKNOWN_ERROR;
        }
    }

    mCameraRunning = native_start_preview(mCameraControlFd);
    if(!mCameraRunning) {
        deinitPreview();
        mPreviewInitialized = false;
        LOGE("startPreview X: native_start_preview failed!");
        return UNKNOWN_ERROR;
    }
    
    //Emit m4mo ioctl found in donut log
    // JPEG Quality == Super fine 
    m4mo_write_8bit( 0x0c, 0x08, 0x62 ) ;
    setLensToBasePosition() ;
    applySettings() ;
      
    LOGD("startPreview X");
    return NO_ERROR;
}

status_t QualcommCameraHardware::startPreview()
{
    LOGD("startPreview E");
    Mutex::Autolock l(&mLock);

    return startPreviewInternal();
}

void QualcommCameraHardware::stopPreviewInternal()
{	  
    LOGD("stopPreviewInternal E: %d", mCameraRunning);
    if (mCameraRunning) {
        // Cancel auto focus.
        if (mMsgEnabled & CAMERA_MSG_FOCUS) {	  
	  LOGD("canceling autofocus");
            cancelAutoFocus();
	}

        LOGD("Stopping preview");
	
        mCameraRunning = !native_stop_preview(mCameraControlFd);
        if (!mCameraRunning && mPreviewInitialized) {
            deinitPreview();
            mPreviewInitialized = false;
        }
        else LOGE("stopPreviewInternal: failed to stop preview");
    }
    LOGD("stopPreviewInternal X: %d", mCameraRunning);
}

void QualcommCameraHardware::stopPreview()
{
    LOGD("stopPreview: E");
    Mutex::Autolock l(&mLock);

    if(mMsgEnabled & CAMERA_MSG_VIDEO_FRAME)
           return;

    stopPreviewInternal();

    LOGD("stopPreview: X");
}

void QualcommCameraHardware::runAutoFocus()
{
  LOGD("runAutoFocus E") ;
    mAutoFocusThreadLock.lock();
    mAutoFocusFd = mCameraControlFd ; //open(MSM_CAMERA_CONTROL, O_RDWR);
    if (mAutoFocusFd < 0) {
        LOGE("autofocus: cannot open %s: %s",
             MSM_CAMERA_CONTROL,
             strerror(errno));
        mAutoFocusThreadRunning = false;
        mAutoFocusThreadLock.unlock();
        return;
    }
    
    mPictureNeedFlash = flashNeeded() && ! isContinuousFocus()  && ! mCameraRecording ;
    
    if( mPictureNeedFlash ) {
	LOGD("flash needed") ;
	startFlashMovie() ;
    }

    mAFCanceled = false ;

    /* This will block until either AF completes or is cancelled. */
    LOGD("af start (fd %d)", mAutoFocusFd);
    //bool status = native_set_afmode(mAutoFocusFd, AF_MODE_AUTO);
    
    
    int32_t value = getParm("focus-mode", focusmode);
    switch( value ) {
      case M4MO_AF_NORMAL :
	m4mo_write_8bit( 0x0a, 0x02, 0x00 ) ;
	m4mo_write_8bit( 0x0a, 0x00, 0x01 ) ;	
	m4mo_write_8bit( 0x0a, 0x01, 0x00 ) ;
	m4mo_write_8bit( 0x0a, 0x10, 0x01 ) ;
	break ;
      case M4MO_AF_MACRO :
	m4mo_write_8bit( 0x0a, 0x02, 0x00 ) ;
	m4mo_write_8bit( 0x0a, 0x00, 0x01 ) ;	
	m4mo_write_8bit( 0x0a, 0x01, 0x01 ) ;
	m4mo_write_8bit( 0x0a, 0x10, 0x02 ) ;
	break;
    }

    if( m4mo_wait_for_value(0x0A, 0x10, 0x00, 50) ) {
      LOGD("AF init OK") ;
    } else {
      LOGE("Failed to wait a AF init!") ;
      mAFCanceled = true ;
    }
    
    // Start AUTOFOCUS
    m4mo_write_8bit( 0x0a, 0x00, 0x01 ) ;
    m4mo_write_8bit( 0x0a, 0x02, 0x01 ) ;
 
    // Wait for focus
    int i = 0 ;
    char f = m4mo_read_8bit( 0x0a, 0x03 ) ;
    while( f == 0 && !mAFCanceled && i < 1000 ) {
	usleep( 20000 ) ;
	f = m4mo_read_8bit( 0x0a, 0x03 ) ;
	i++ ;
    }
    LOGD("af done: %d", f );
    
    if( mPictureNeedFlash ) {
	LOGD("flash needed") ;
	stopFlashMovie() ;
    }
    
    // Release AF 
    m4mo_write_8bit( 0x0a, 0x02, 0x00 ) ;
    if( mFaceDetection == M4MO_FACE_DETECTION_ON ) {
	if( ! m4mo_wait_for_value( 0x00, 0x0C, 0x04, 50 ) ) {
	  LOGE("Failed to wait changing to face detection status!") ;
	}
    } else {
	if( ! m4mo_wait_for_value( 0x00, 0x0C, 0x02, 50 ) ) {
	  LOGE("Failed to wait changing to monitor status!") ;
	}      
    }
    
    mAutoFocusThreadRunning = false;
    //close(mAutoFocusFd);
    mAutoFocusFd = -1;
    mAutoFocusThreadLock.unlock();

    if (mMsgEnabled & CAMERA_MSG_FOCUS)
        mNotifyCb(CAMERA_MSG_FOCUS, ( !mAFCanceled && f == 1 ), 0, mCallbackCookie);
  LOGD("runAutoFocus X") ;
    
}

status_t QualcommCameraHardware::cancelAutoFocus()
{
    LOGD("cancelAutoFocus E");
    //native_cancel_afmode(mCameraControlFd, mAutoFocusFd);
    mAFCanceled = true ;
    LOGD("cancelAutoFocus X");

    /* Needed for eclair camera PAI */
    return NO_ERROR;
}

void *auto_focus_thread(void *user)
{
    LOGD("auto_focus_thread E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->runAutoFocus();
    }
    else LOGW("not starting autofocus: the object went away!");
    LOGD("auto_focus_thread X");
    return NULL;
}

status_t QualcommCameraHardware::autoFocus()
{
    LOGD("autoFocus E");
    Mutex::Autolock l(&mLock);

    if (mCameraControlFd < 0) {
        LOGE("not starting autofocus: main control fd %d", mCameraControlFd);
        return UNKNOWN_ERROR;
    }

    /* Not sure this is still needed with new APIs .. 
    if (mMsgEnabled & CAMERA_MSG_FOCUS) {
        LOGW("Auto focus is already in progress");
        return NO_ERROR;
        // No idea how to rewrite this
        //return mAutoFocusCallback == af_cb ? NO_ERROR : INVALID_OPERATION;
    }*/

    {
        mAutoFocusThreadLock.lock();
        if (!mAutoFocusThreadRunning) {
            // Create a detatched thread here so that we don't have to wait
            // for it when we cancel AF.
            pthread_t thr;
            pthread_attr_t attr;
            pthread_attr_init(&attr);
            pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
            mAutoFocusThreadRunning =
                !pthread_create(&thr, &attr,
                                auto_focus_thread, NULL);
            if (!mAutoFocusThreadRunning) {
                LOGE("failed to start autofocus thread");
                mAutoFocusThreadLock.unlock();
                return UNKNOWN_ERROR;
            }
        }
        mAutoFocusThreadLock.unlock();
    }

    LOGD("autoFocus X");
    return NO_ERROR;
}

void m4mo_write_8bit( int fd, char category, char byte, char value ) 
{
  ioctl_m4mo_info_8bit cmd ;
  cmd.category = category ;
  cmd.byte = byte ;
  cmd.value = value ;
  if ((ioctl(fd, MSM_CAM_IOCTL_M4MO_I2C_WRITE_8BIT, &cmd)) < 0)
    LOGE("write_8bit : ioctl fd %d error %s\n",
	fd,
	strerror(errno));

}

unsigned char m4mo_read_8bit( int fd, char category, char byte ) 
{
  ioctl_m4mo_info_8bit cmd ;
  cmd.category = category ;
  cmd.byte = byte ;
  if ((ioctl(fd, MSM_CAM_IOCTL_M4MO_I2C_READ_8BIT, &cmd)) < 0)
    LOGE("write_8bit : ioctl fd %d error %s\n",
	fd,
	strerror(errno));
  return cmd.value ;
}

bool QualcommCameraHardware::m4mo_wait_for_value( char category, char byte, char value, int nbtry ) 
{
  bool res = false; 
  int i = 0 ;
  while( i < nbtry ) {
    char v = m4mo_read_8bit( category, byte ) ;
    if( v == value ) {
      return true; 
    }
    i++ ;
    usleep( 10000 ) ;
  }
  return res ;
}

bool QualcommCameraHardware::m4mo_switch_to_param_mode()
{
        // switch to SETPARM mode 
      m4mo_write_8bit( 0x00, 0x0B, 0x01 ) ;
      
      if( m4mo_wait_for_value( 0x00, 0x0B, 0x01, 50 ) ) {
	LOGD("WAIT FOR SETPARM SUCCESS");
	return true;
      } else {
	LOGD("FAILED WAIT SETPARM") ;
	return false ;
      }
      
}

void QualcommCameraHardware::m4mo_switch_to_monitor_mode()
{
  m4mo_write_8bit( 0x00, 0x0B, 0x02 ) ;
}
    
void QualcommCameraHardware::m4mo_write_8bit( char category, char byte, char value )
{
  android::m4mo_write_8bit( mCameraControlFd, category, byte, value ) ;
}

void QualcommCameraHardware::m4mo_get_firmware_version()
{
  char minor = m4mo_read_8bit( 0x00, 0x01 ) ;
  char major = m4mo_read_8bit( 0x00, 0x02 ) ;
}

unsigned char QualcommCameraHardware::m4mo_read_8bit( char category, char byte ) 
{
  return android::m4mo_read_8bit( mCameraControlFd, category, byte ) ;
}

void QualcommCameraHardware::runSnapshotThread(void *data)
{
    LOGD("runSnapshotThread E");
   
      bool needFlash = flashNeeded() ;
      if( needFlash ) {
	  startFlash() ;
	  m4mo_write_8bit( 0x03, 0x00, 0x00 ) ;
	  m4mo_write_8bit( 0x06, 0x00, 0x00 ) ;
	  m4mo_write_8bit( 0x0b, 0x0b, 0x01 ) ;
	  m4mo_write_8bit( 0x03, 0x16, 0x05 ) ;
	  m4mo_write_8bit( 0x03, 0x17, 0x35 ) ;
	  usleep( 30000 ) ;

      }
    
	m4mo_write_8bit( 0x0c, 0x00, 0x00 ) ;
	m4mo_write_8bit( 0x02, 0x00, 0x08 ) ;
	
	// Requested image size ;
	int32_t value = getParm("picture-size", picturesize) ;
	if( value != NOT_FOUND ) {
	  switch( value ) {
	    case M4MO_SHOT_3M_SIZE :
	      LOGD("requested 3M picture") ;
	      m4mo_write_8bit( 0x02, 0x04, 0x05 ) ;
	      break ;
	    case M4MO_SHOT_2M_SIZE :
	      LOGD("requested 2M picture") ;
	      m4mo_write_8bit( 0x02, 0x04, 0x04 ) ;
	      break ;	    
	    case M4MO_SHOT_1024_768_SIZE :
	      LOGD("requested 1M picture") ;
	      m4mo_write_8bit( 0x02, 0x04, 0x16 ) ;
	      break ;	   
	    case M4MO_SHOT_5M_SIZE :
	    default:
	      LOGD("requested 5M picture") ;
	      m4mo_write_8bit( 0x02, 0x04, 0x06 ) ;
	  }
	  
	}
	
	m4mo_write_8bit( 0x02, 0x25, 0x01 ) ;
	m4mo_write_8bit( 0x00, 0x11, 0x08 ) ;
	m4mo_write_8bit( 0x00, 0x12, 0x01 ) ;
	usleep( 20000 ) ;
	
	   m4mo_write_8bit( 0x00, 0x0b, 0x03 );

	if (native_start_snapshot(mCameraControlFd)) {
	  if( needFlash ) {
	//    m4mo_write_8bit( 0x00, 0x0b, 0x03 ) ;
	    usleep( 500000 ) ;
	    stopFlash() ;
	  }
	// guessed from logs 
	notifyShutter() ;

	
        receiveRawPicture();
    }
    else {
      if( needFlash ) {
	    stopFlash() ;
	  }
        LOGE("main: native_start_snapshot failed!");
    }

    mSnapshotThreadWaitLock.lock();
    mSnapshotThreadRunning = false;
    mSnapshotThreadWait.signal();
    mSnapshotThreadWaitLock.unlock();

    LOGD("runSnapshotThread X");
}

void *snapshot_thread(void *user)
{
    LOGD("snapshot_thread E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->runSnapshotThread(user);
    }
    else LOGW("not starting snapshot thread: the object went away!");
    LOGD("snapshot_thread X");
    return NULL;
}

status_t QualcommCameraHardware::takePicture()
{
    LOGD("takePicture: E");
    Mutex::Autolock l(&mLock);

    // Wait for old snapshot thread to complete.
    mSnapshotThreadWaitLock.lock();
    while (mSnapshotThreadRunning) {
        LOGD("takePicture: waiting for old snapshot thread to complete.");
        mSnapshotThreadWait.wait(mSnapshotThreadWaitLock);
        LOGD("takePicture: old snapshot thread completed.");
    }

    stopPreviewInternal();

    if (!initRaw(mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE)) { /* not sure if this is right */
        LOGE("initRaw failed.  Not taking picture.");
        return UNKNOWN_ERROR;
    }
    


    mShutterLock.lock();
    mShutterPending = true;
    mShutterLock.unlock();

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    mSnapshotThreadRunning = !pthread_create(&mSnapshotThread,
                                             &attr,
                                             snapshot_thread,
                                             NULL);
    mSnapshotThreadWaitLock.unlock();

    LOGD("takePicture: X");
    return mSnapshotThreadRunning ? NO_ERROR : UNKNOWN_ERROR;
}

status_t QualcommCameraHardware::cancelPicture()
{
    LOGD("cancelPicture: EX");
    return NO_ERROR;
}

status_t QualcommCameraHardware::setParameters(
        const CameraParameters& params)
{
    LOGD("setParameters: E params = %p", &params);

    Mutex::Autolock l(&mLock);

    // Set preview size.
    preview_size_type *ps = preview_sizes;
    {
        int width, height;
        params.getPreviewSize(&width, &height);
        LOGD("requested size %d x %d", width, height);
        // Validate the preview size
        size_t i;
        for (i = 0; i < PREVIEW_SIZE_COUNT; ++i, ++ps) {
            if (width == ps->width && height == ps->height)
                break;
        }
        if (i == PREVIEW_SIZE_COUNT) {
            LOGE("Invalid preview size requested: %dx%d",
                 width, height);
            return BAD_VALUE;
        }
    }
    mPreviewWidth = mDimension.display_width = ps->width;
    mPreviewHeight = mDimension.display_height = ps->height;

    LOGD("mPreviewWidth %d, mPreviewHeight %d", mPreviewWidth, mPreviewHeight ) ;
    
    // FIXME: validate snapshot sizes,
    params.getPictureSize(&mRawWidth, &mRawHeight);
    
    mRawWidth = (mRawWidth+1) & ~1 ;
    mRawHeight = (mRawHeight+1) & ~1 ;
    
    
    mDimension.picture_width = mRawWidth;
    mDimension.picture_height = mRawHeight;

    // Set up the jpeg-thumbnail-size parameters.
    {
        int val;

        val = params.getInt("jpeg-thumbnail-width");
        if (val < 0) {
            mDimension.ui_thumbnail_width= THUMBNAIL_WIDTH;
            LOGW("jpeg-thumbnail-width is not specified: defaulting to %d",
                 THUMBNAIL_WIDTH);
        }
        else mDimension.ui_thumbnail_width = val;
	LOGD("jpeg-thumbnail-width %d", val ) ;

        val = params.getInt("jpeg-thumbnail-height");
        if (val < 0) {
            mDimension.ui_thumbnail_height= THUMBNAIL_HEIGHT;
            LOGW("jpeg-thumbnail-height is not specified: defaulting to %d",
                 THUMBNAIL_HEIGHT);
        }
        else mDimension.ui_thumbnail_height = val;
        LOGD("jpeg-thumbnail-height %d", val ) ;
    }

    mParameters = params;

    //setAntibanding();
    if( mCameraRunning ) { 
	applySettings(); 
    }
    
    //
    // FIXME: set nightshot and luma adaptatiom

    LOGD("setParameters: X");
    return NO_ERROR ;
}


void QualcommCameraHardware::applySettings()
{
      setFocusMode() ;
      setEffect();
      setWhiteBalance();
      setZoom() ;
      setIso() ;
      setExposure() ;
      setSharpness() ;
      setSaturation() ;
      setContrast() ;  
      setAutoExposure() ;      
      setFaceDetection() ;
      setWideDynamicRange() ;      
      setImageStabilizationControl() ;
      setAEWB() ;
      setSceneMode() ;
}


CameraParameters QualcommCameraHardware::getParameters() const
{
    LOGD("getParameters: EX");
    return mParameters;
}

extern "C" sp<CameraHardwareInterface> openCameraHardware()
{
    LOGD("openCameraHardware: call createInstance");
    return QualcommCameraHardware::createInstance();
}

wp<QualcommCameraHardware> QualcommCameraHardware::singleton;

// If the hardware already exists, return a strong pointer to the current
// object. If not, create a new hardware object, put it in the singleton,
// and return it.
sp<CameraHardwareInterface> QualcommCameraHardware::createInstance()
{
    LOGD("createInstance: E");

    Mutex::Autolock lock(&singleton_lock);

    // Wait until the previous release is done.
    while (singleton_releasing) {
        LOGD("Wait for previous release.");
        singleton_wait.wait(singleton_lock);
    }

    if (singleton != 0) {
        sp<CameraHardwareInterface> hardware = singleton.promote();
        if (hardware != 0) {
            LOGD("createInstance: X return existing hardware=%p", &(*hardware));
            return hardware;
        }
    }

    {
        struct stat st;
        int rc = stat("/dev/oncrpc", &st);
        if (rc < 0) {
            LOGD("createInstance: X failed to create hardware: %s", strerror(errno));
            return NULL;
        }
    }

    QualcommCameraHardware *cam = new QualcommCameraHardware();
    sp<QualcommCameraHardware> hardware(cam);
    singleton = hardware;

    cam->initDefaultParameters();
    cam->startCamera();
    
    LOGD("createInstance: X created hardware=%p", &(*hardware));
    return hardware;
}

// For internal use only, hence the strong pointer to the derived type.
sp<QualcommCameraHardware> QualcommCameraHardware::getInstance()
{
    sp<CameraHardwareInterface> hardware = singleton.promote();
    if (hardware != 0) {
        //    LOGD("getInstance: X old instance of hardware");
        return sp<QualcommCameraHardware>(static_cast<QualcommCameraHardware*>(hardware.get()));
    } else {
        LOGD("getInstance: X new instance of hardware");
        return sp<QualcommCameraHardware>();
    }
}

static ssize_t previewframe_offset = 3;

void QualcommCameraHardware::receivePreviewFrame(struct msm_frame_t *frame)
{
//    LOGD("receivePreviewFrame E");

    if (!mCameraRunning) {
        LOGE("ignoring preview callback--camera has been stopped");
        return;
    }

    // Why is this here?
    /*mCallbackLock.lock();
    preview_callback pcb = mPreviewCallback;
    void *pdata = mPreviewCallbackCookie;
    recording_callback rcb = mRecordingCallback;
    void *rdata = mRecordingCallbackCookie;
    mCallbackLock.unlock(); */

    // Find the offset within the heap of the current buffer.
    //ssize_t offset =
    /////    (ssize_t)frame->buffer - (ssize_t)mPreviewHeap->mHeap->base();
    //offset /= mPreviewFrameSize;
    
         if ((unsigned int)mPreviewHeap->mHeapnew[previewframe_offset]->base() !=
                         (unsigned int)frame->buffer)
             for (previewframe_offset = 0; previewframe_offset < 4; previewframe_offset++) {
                 if ((unsigned int)mPreviewHeap->mHeapnew[previewframe_offset]->base() ==
                     (unsigned int)frame->buffer)
                     break;
            }
    

    mInPreviewCallback = true;
    if (mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME) {
	//LOGD("mDataCb(%p)", mPreviewHeap->mBuffers[previewframe_offset]->getHeap()->getBase() ) ;
        mDataCb(CAMERA_MSG_PREVIEW_FRAME, mPreviewHeap->mBuffers[previewframe_offset], mCallbackCookie);
    }

    if (mMsgEnabled & CAMERA_MSG_VIDEO_FRAME) {
        Mutex::Autolock rLock(&mRecordFrameLock);
        mDataCbTimestamp(systemTime(), CAMERA_MSG_VIDEO_FRAME, mPreviewHeap->mBuffers[previewframe_offset], mCallbackCookie); /* guess? */
        //mDataCb(CAMERA_MSG_VIDEO_FRAME, mPreviewHeap->mBuffers[offset], mCallbackCookie);

        if (mReleasedRecordingFrame != true) {
            LOGV("block for release frame request/command");
            mRecordWait.wait(mRecordFrameLock);
        }
        mReleasedRecordingFrame = false;
    }

    /*if(mMsgEnabled & CAMERA_MSG_VIDEO_IMAGE) {
        Mutex::Autolock rLock(&mRecordFrameLock);
        rcb(systemTime(), mPreviewHeap->mBuffers[offset], rdata);
        if (mReleasedRecordingFrame != true) {
            LOGD("block for release frame request/command");
            mRecordWait.wait(mRecordFrameLock);
        }
        mReleasedRecordingFrame = false;
    }*/
    mInPreviewCallback = false;
         previewframe_offset--;
         previewframe_offset &= 3;
//    LOGD("receivePreviewFrame X");
}



status_t QualcommCameraHardware::startRecording()
{
    LOGD("startRecording E");
    Mutex::Autolock l(&mLock);

    mReleasedRecordingFrame = false;
    mCameraRecording = true;

    mVideoFlashRunning = flashNeeded() ;
    if( mVideoFlashRunning ) {
    	startFlashMovie() ;
    }

    return startPreviewInternal();
}

void QualcommCameraHardware::stopRecording()
{
    LOGD("stopRecording: E");
    Mutex::Autolock l(&mLock);

    {
        mRecordFrameLock.lock();
        mReleasedRecordingFrame = true;
        mRecordWait.signal();
        mRecordFrameLock.unlock();

        mCameraRecording = false;

        if( mVideoFlashRunning ) {
        	stopFlashMovie() ;
        	mVideoFlashRunning = false ;
        }


        if(mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME) {
            LOGD("stopRecording: X, preview still in progress");
            return;
        }
    }

    stopPreviewInternal();
    LOGD("stopRecording: X");
}

void QualcommCameraHardware::releaseRecordingFrame(
       const sp<IMemory>& mem __attribute__((unused)))
{
    LOGV("releaseRecordingFrame E");
    Mutex::Autolock l(&mLock);
    Mutex::Autolock rLock(&mRecordFrameLock);
    mReleasedRecordingFrame = true;
    mRecordWait.signal();
    LOGV("releaseRecordingFrame X");
}

bool QualcommCameraHardware::recordingEnabled()
{
    return (mCameraRunning && mCameraRecording);
}

void QualcommCameraHardware::notifyShutter()
{
    mShutterLock.lock();
    if (mShutterPending && (mMsgEnabled & CAMERA_MSG_SHUTTER)) {
        mNotifyCb(CAMERA_MSG_SHUTTER, 0, 1, mCallbackCookie);
        mShutterPending = false;
    }
    mShutterLock.unlock();
}
/*
static void receive_shutter_callback()
{
    LOGD("receive_shutter_callback: E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->notifyShutter();
    }
    LOGD("receive_shutter_callback: X");
}
*/

void QualcommCameraHardware::receiveRawPicture()
{
    LOGD("receiveRawPicture: E");

    
    
    if (mMsgEnabled & CAMERA_MSG_RAW_IMAGE) {
    int nbtry = 0 ;
    char ret = 0 ;
    do {
      ret = m4mo_read_8bit( 0x00, 0x0c ) ;
      nbtry++ ;
      usleep( 30000 ) ;
    } while( ret != 7 && nbtry < 50 ) ;
    
    char r = m4mo_read_8bit( 0x00, 0x10 ) ;
    m4mo_write_8bit( 0x0c, 0x04, 0x01 ) ;
    m4mo_write_8bit( 0x0c, 0x05, 0x01 ) ;

      LOGD("before native_get_picture") ;
        if(native_get_picture(mCameraControlFd, &mCrop) == false) {
            LOGE("getPicture failed!");
            return;
        }
        
        unsigned char r1  = m4mo_read_8bit( 0x0c, 0x10 ) ;
	unsigned char r2  = m4mo_read_8bit( 0x0c, 0x11 ) ;
	unsigned char r3  = m4mo_read_8bit( 0x0c, 0x12 ) ;
	unsigned char r4  = m4mo_read_8bit( 0x0c, 0x13 ) ;
	
	int size = (r1<<24) + (r2<<16) + (r3<<8) + r4 ;
        LOGD("Capture Image Size == %d" , size ) ;
	mJpegSize = size ;
	
	char r5  = m4mo_read_8bit( 0x00, 0x01 ) ;
	char r6  = m4mo_read_8bit( 0x00, 0x02 ) ;
	
	char r7  = m4mo_read_8bit( 0x07, 0x00 ) ;
	char r8  = m4mo_read_8bit( 0x07, 0x01 ) ;
	char r9  = m4mo_read_8bit( 0x07, 0x02 ) ;
	char r10 = m4mo_read_8bit( 0x07, 0x03 ) ;
	char r11 = m4mo_read_8bit( 0x07, 0x04 ) ;
	char r12 = m4mo_read_8bit( 0x07, 0x05 ) ;
	char r13 = m4mo_read_8bit( 0x07, 0x06 ) ;
	char r14 = m4mo_read_8bit( 0x07, 0x07 ) ;
	
	char r15 = m4mo_read_8bit( 0x07, 0x28 ) ;
	char r16 = m4mo_read_8bit( 0x07, 0x29 ) ;
	
	char r17 = m4mo_read_8bit( 0x03, 0x01 ) ;
	
	char r18 = m4mo_read_8bit( 0x01, 0x04 ) ;
	
	char r19 = m4mo_read_8bit( 0x02, 0x0f ) ;
	char r20 = m4mo_read_8bit( 0x02, 0x11 ) ;
	
	char r21 = m4mo_read_8bit( 0x06, 0x02 ) ;
        
	LOGD("native_get_picture done") ;
        // By the time native_get_picture returns, picture is taken. Call
        // shutter callback if cam config thread has not done that.				  
        mDataCb(CAMERA_MSG_RAW_IMAGE, mRawHeap->mBuffers[0], mCallbackCookie);
    }
    else LOGD("Raw-picture callback was canceled--skipping.");

    if (mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE) {
       // mJpegSize = 0;
        if (LINK_jpeg_encoder_init()) {
            if(native_jpeg_encode()) {
                LOGD("receiveRawPicture: X (success)");
                return;
            }
            LOGE("jpeg encoding failed");
        }
        else LOGE("receiveRawPicture X: jpeg_encoder_init failed.");
    }
    else LOGD("JPEG callback is NULL, not encoding image.");

    if( mRawInitialized ) {
    	deinitRaw();
    }
    
    LOGD("receiveRawPicture: X");
}

void QualcommCameraHardware::receiveJpegPictureFragment(
    uint8_t *buff_ptr, uint32_t buff_size)
{
    uint32_t remaining = mJpegHeap->mHeap->virtualSize();
    remaining -= mJpegSize;
    uint8_t *base = (uint8_t *)mJpegHeap->mHeap->base();

    LOGD("receiveJpegPictureFragment size %d", buff_size);
    if (buff_size > remaining) {
        LOGE("receiveJpegPictureFragment: size %d exceeds what "
             "remains in JPEG heap (%d), truncating",
             buff_size,
             remaining);
        buff_size = remaining;
    }
    memcpy(base + mJpegSize, buff_ptr, buff_size);
    mJpegSize += buff_size;
}

void QualcommCameraHardware::receiveJpegPicture(void)
{
    LOGD("receiveJpegPicture: E image (%d uint8_ts out of %d)",
         mJpegSize, mJpegHeap->mBufferSize);

    LOGD("mJpegHeap->mFrameOffset %d", mJpegHeap->mFrameOffset ) ;
    
    int index = 0, rc;

    if (mMsgEnabled & CAMERA_MSG_COMPRESSED_IMAGE) {
        // The reason we do not allocate into mJpegHeap->mBuffers[offset] is
        // that the JPEG image's size will probably change from one snapshot
        // to the next, so we cannot reuse the MemoryBase object.
        sp<MemoryBase> buffer = new
            MemoryBase(mJpegHeap->mHeap,
                       index * mJpegHeap->mBufferSize +
                       mJpegHeap->mFrameOffset,
                       mJpegSize);

        mDataCb(CAMERA_MSG_COMPRESSED_IMAGE, buffer, mCallbackCookie);
        buffer = NULL;
    }
    else LOGD("JPEG callback was cancelled--not delivering image.");

    LINK_jpeg_encoder_join();
    deinitRaw();

    LOGD("receiveJpegPicture: X callback done.");
}

bool QualcommCameraHardware::previewEnabled()
{
    Mutex::Autolock l(&mLock);
    return (mCameraRunning && (mMsgEnabled & CAMERA_MSG_PREVIEW_FRAME));
}

int QualcommCameraHardware::getParm(
    const char *parm_str, const struct str_map *const parm_map)
{
    // Check if the parameter exists.
    const char *str = mParameters.get(parm_str);
    if (str == NULL) return NOT_FOUND;

    // Look up the parameter value.
    return attr_lookup(parm_map, str);
}

bool QualcommCameraHardware::isContinuousFocus()
{
	const char *str = mParameters.get("focus-mode") ;
	bool res = false ;

	if( strncmp( str, "auto", 4 ) == 0 ) {
		res = true ;
	}
	return res ;
}

void QualcommCameraHardware::setExposure()
{
      int32_t value = mParameters.getInt("exposure-compensation");
LOGD("exposure == %s", mParameters.get("exposure-compensation") ) ;
    // same as before, do nothing
    if( value == mExposure ) 
      return ;

    switch( value ) {
	case M4MO_EV_MINUS_4:
	  m4mo_write_8bit(0x06, 0x04, 0xFC);
	  break;
	case M4MO_EV_MINUS_3:
	  m4mo_write_8bit(0x06, 0x04, 0xFD);
	  break;
	case M4MO_EV_MINUS_2:
	  m4mo_write_8bit(0x06, 0x04, 0xFE);
	  break;
	case M4MO_EV_MINUS_1:
	  m4mo_write_8bit(0x06, 0x04, 0xFF);
	  break;
	case M4MO_EV_DEFAULT:
	  m4mo_write_8bit(0x06, 0x04, 0x00);
	  break;
	case M4MO_EV_PLUS_1:
	  m4mo_write_8bit(0x06, 0x04, 0x01);
	  break;
	case M4MO_EV_PLUS_2:
	  m4mo_write_8bit(0x06, 0x04, 0x02);
	  break;
	case M4MO_EV_PLUS_3:
	  m4mo_write_8bit(0x06, 0x04, 0x03);
	  break;
	case M4MO_EV_PLUS_4:
	  m4mo_write_8bit(0x06, 0x04, 0x04);
	  break;
	default:
		LOGV("Invalid exposure-compensation value %d", value ) ;
		return ;
      }
      mExposure = value ;
}


void QualcommCameraHardware::setSharpness()
{
      int32_t value = mParameters.getInt("sharpness");
LOGD("sharpness == %s", mParameters.get("sharpness")) ;
    // same as before, do nothing
    if( value == mSharpness ) 
      return ;
      switch( value ) {
    case M4MO_SHARPNESS_MINUS_3:
		m4mo_write_8bit(0x02, 0x11, 0x1C);
		m4mo_write_8bit(0x02, 0x12, 0x00);
		break;
    case M4MO_SHARPNESS_MINUS_2:
	  m4mo_write_8bit(0x02, 0x11, 0x3E);
	  m4mo_write_8bit(0x02, 0x12, 0x00);
	  break;
	case M4MO_SHARPNESS_MINUS_1:
	  m4mo_write_8bit(0x02, 0x11, 0x5F);
	  m4mo_write_8bit(0x02, 0x12, 0x00);
	  break;
	case M4MO_SHARPNESS_DEFAULT:
	  m4mo_write_8bit(0x02, 0x11, 0x80);
	  m4mo_write_8bit(0x02, 0x12, 0x01);
	  break;
	case M4MO_SHARPNESS_PLUS_1:
	  m4mo_write_8bit(0x02, 0x11, 0xA1);
	  m4mo_write_8bit(0x02, 0x12, 0x00);
	  break;
	case M4MO_SHARPNESS_PLUS_2:
	  m4mo_write_8bit(0x02, 0x11, 0xC2);
	  m4mo_write_8bit(0x02, 0x12, 0x00);
	  break;	
	case M4MO_SHARPNESS_PLUS_3:
	  m4mo_write_8bit(0x02, 0x11, 0xE4);
	  m4mo_write_8bit(0x02, 0x12, 0x00);
	  break;
	default:
	  LOGV("Invalid sharpness value %d" , value ) ;
	  return ;
      }
      mSharpness = value ;
}

void QualcommCameraHardware::setSaturation()
{
    int32_t value = mParameters.getInt("saturation");
 LOGD("saturation == %s", mParameters.get("saturation") ) ;
    // same as before, do nothing
    if( value == mSaturation ) 
      return ;
    
      switch( value ) {
  	case M4MO_SATURATION_MINUS_3:
  	  m4mo_write_8bit(0x02, 0x0F, 0x1C);
  	  m4mo_write_8bit(0x02, 0x10, 0x00);
  	  break;
	case M4MO_SATURATION_MINUS_2:
	  m4mo_write_8bit(0x02, 0x0F, 0x3E);
	  m4mo_write_8bit(0x02, 0x10, 0x00);
	  break;
	case M4MO_SATURATION_MINUS_1:
	  m4mo_write_8bit(0x02, 0x0F, 0x5F);
	  m4mo_write_8bit(0x02, 0x10, 0x00);
	  break;
	case M4MO_SATURATION_DEFAULT:
	  m4mo_write_8bit(0x02, 0x0F, 0x80);
	  m4mo_write_8bit(0x02, 0x10, 0x01);
	  break;
	case M4MO_SATURATION_PLUS_1:
	  m4mo_write_8bit(0x02, 0x0F, 0xA1);
	  m4mo_write_8bit(0x02, 0x10, 0x00);
	  break;
	case M4MO_SATURATION_PLUS_2:
	  m4mo_write_8bit(0x02, 0x0F, 0xC2);
	  m4mo_write_8bit(0x02, 0x10, 0x00);
	  break;
	case M4MO_SATURATION_PLUS_3:
	  m4mo_write_8bit(0x02, 0x0F, 0xE4);
	  m4mo_write_8bit(0x02, 0x10, 0x00);
	  break;
	default:
	  LOGE("Invalid contrast value %d", saturation) ;
	  return ;
      }
      mSaturation = value ;

}

void QualcommCameraHardware::setWideDynamicRange()
{
    int32_t value = getParm("wide-dynamic-range", wdr);
    LOGD("wide-dynamic-range == %s", mParameters.get("wide-dynamic-range") ) ;
    // same as before, do nothing
    if( value == mWideDynamicRange ) 
      return ;
    
    if( value != NOT_FOUND ) {
      switch( value ) {
	case M4MO_WDR_OFF:
	  switch( mImageStabilizationControl ) {
	    case M4MO_ISC_STILL_ON :
	      m4mo_write_8bit(0x0C, 0x00, 0x06);
	      break ;
	    case M4MO_ISC_STILL_AUTO :
	      m4mo_write_8bit(0x0C, 0x00, 0x02);
	      break ;
	    default: 
	      m4mo_write_8bit(0x0C, 0x00, 0x00);
	  }
	  break;
	case M4MO_WDR_ON:
	  m4mo_write_8bit(0x0C, 0x00, 0x04);
	  mImageStabilizationControl = M4MO_ISC_STILL_OFF ;
	  mParameters.set("image-stabilization-control", "off") ;
	  break;
	default:
	    LOGE("Invalid wide-dynamic-range value") ;
      }
      mWideDynamicRange = value ;
    }else {
      LOGE("wide-dynamic-range %s not found", mParameters.get("wide-dynamic-range") ) ;
    }
}

void QualcommCameraHardware::setImageStabilizationControl()
{
    int32_t value = getParm("image-stabilization-control", isc);
    LOGD("image-stabilization-control == %s", mParameters.get("image-stabilization-control") ) ;
    // same as before, do nothing
    if( value == mImageStabilizationControl ) 
      return ;
    
    if( value != NOT_FOUND ) {
      switch( value ) {
	case M4MO_ISC_STILL_OFF:
	  if( mWideDynamicRange == M4MO_WDR_ON ) {
	    m4mo_write_8bit(0x0C, 0x00, 0x04);
	  } else {
	    m4mo_write_8bit(0x0C, 0x00, 0x00);
	  }
	  break;
	case M4MO_ISC_STILL_ON:
	  m4mo_write_8bit(0x0C, 0x00, 0x06);
	  mWideDynamicRange = M4MO_WDR_OFF ;
	  mParameters.set("wide-dynamic-range", "off") ;
	  break;
	case M4MO_ISC_STILL_AUTO:
	  m4mo_write_8bit(0x0C, 0x00, 0x02);
	  break;
	default:
	    LOGE("Invalid image-stabilization-control value") ;
      }
      mImageStabilizationControl = value ;
    }else {
      LOGE("image-stabilization-control %s not found", mParameters.get("image-stabilization-control") ) ;
    }
}

void QualcommCameraHardware::setFocusMode()
{
    int32_t value = getParm("focus-mode", focusmode );
    if( value == mFocusMode ) 
      return ;
    if( value != NOT_FOUND ) {
      mFocusMode = value ;
      setLensToBasePosition() ;
    }
    
}

void QualcommCameraHardware::setFaceDetection()
{
    int32_t value = getParm("face-detection", facedetection);
    LOGD("face-detection == %s", mParameters.get("face-detection") ) ;
    // same as before, do nothing
    if( value == mFaceDetection ) 
      return ;
    
    if( value != NOT_FOUND ) {
      m4mo_switch_to_param_mode() ;
      switch( value ) {
	case M4MO_FACE_DETECTION_OFF:
	  m4mo_write_8bit(0x09, 0x01, 0x00);
	  m4mo_write_8bit(0x09, 0x00, 0x00);
	  break;
	case M4MO_FACE_DETECTION_ON:
	  m4mo_write_8bit(0x09, 0x01, 0x01);
	  m4mo_write_8bit(0x09, 0x00, 0x11);
	  break;
	default:
	    LOGE("Invalid face-detection value") ;
      }
      mFaceDetection = value ;
      m4mo_switch_to_monitor_mode() ;
    }else {
      LOGE("face-detection %s not found", mParameters.get("face-detection") ) ;
    }
}

void QualcommCameraHardware::setAEWB()
{
      int32_t value = getParm("aewb-lock", aewb);
LOGD("aewb-lock == %s", mParameters.get("aewb-lock")) ;
    // same as before, do nothing
    if( value == mAEWB ) 
      return ;
    if( value != NOT_FOUND ) {    
      switch( value ) {
	case M4MO_AE_LOCK_AWB_LOCK:
	  m4mo_write_8bit(0x03, 0x00, 0x01);
	  m4mo_write_8bit(0x06, 0x00, 0x01);
	  break;
	case M4MO_AE_LOCK_AWB_UNLOCK:
	  m4mo_write_8bit(0x03, 0x00, 0x01);
	  m4mo_write_8bit(0x06, 0x00, 0x00);
	  break;
	case M4MO_AE_UNLOCK_AWB_LOCK:
	  m4mo_write_8bit(0x03, 0x00, 0x00);
	  m4mo_write_8bit(0x06, 0x00, 0x01);
	  break;
	case M4MO_AE_UNLOCK_AWB_UNLOCK:
	  m4mo_write_8bit(0x03, 0x00, 0x00);
	  m4mo_write_8bit(0x06, 0x00, 0x00);
	  break;
	
      }
      mAEWB = value ;
    } else {
      LOGE("aewb-lock %s not found", mParameters.get("aewb-lock") ) ;
    }
}

void QualcommCameraHardware::setAutoExposure()
{
    int32_t value = getParm("meter-mode", autoexposure);
    LOGD("autoexposure == %s", mParameters.get("meter-mode") ) ;
    // same as before, do nothing
    if( value == mAutoExposure ) 
      return ;
    
    if( value != NOT_FOUND ) {
      switch( value ) {
	case M4MO_PHOTOMETRY_AVERAGE:
	  m4mo_write_8bit(0x03, 0x01, 0x01);
	  break;
	case M4MO_PHOTOMETRY_CENTER:
	  m4mo_write_8bit(0x03, 0x01, 0x03);
	  break;
	case M4MO_PHOTOMETRY_SPOT:
	  m4mo_write_8bit(0x03, 0x01, 0x05);
	  break;
	default:
	    LOGE("Invalid autoexposure value") ;
      }
      mAutoExposure = value ;
    }else {
      LOGE("autoexposure %s not found", mParameters.get("meter-mode") ) ;
    }
}

void QualcommCameraHardware::setContrast()
{
    int32_t value = mParameters.getInt("contrast");
    LOGD("contrast == %s", mParameters.get("contrast") ) ;
    // same as before, do nothing
    if( value == mContrast ) 
      return ;
    
      m4mo_switch_to_param_mode() ;
      switch( value ) {
  	case M4MO_CONTRAST_MINUS_3:
  	  m4mo_write_8bit(0x01, 0x04, 0x1C);
  	  break;
    case M4MO_CONTRAST_MINUS_2:
	  m4mo_write_8bit(0x01, 0x04, 0x3E);
	  break;
	case M4MO_CONTRAST_MINUS_1:
	  m4mo_write_8bit(0x01, 0x04, 0x5F);
	  break;
	case M4MO_CONTRAST_DEFAULT:
	  m4mo_write_8bit(0x01, 0x04, 0x80);
	  break;
	case M4MO_CONTRAST_PLUS_1:
	  m4mo_write_8bit(0x01, 0x04, 0xA1);
	  break;
	case M4MO_CONTRAST_PLUS_2:
	  m4mo_write_8bit(0x01, 0x04, 0xC2);
	  break;
	case M4MO_CONTRAST_PLUS_3:
	  m4mo_write_8bit(0x01, 0x04, 0xE4);
	  break;
	default:
	    LOGE("Invalid contrast value %d", value) ;
	    return ;
      }
      mContrast = value ;
      m4mo_switch_to_monitor_mode() ;

}

void QualcommCameraHardware::setIso()
{
    int32_t value = getParm("iso", iso);

    // same as before, do nothing
    if( value == mIso ) 
      return ;
    
    if( value != NOT_FOUND ) {
      switch( value ) {
	case M4MO_ISO_AUTO:
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  break;
	case M4MO_ISO_50:
	  m4mo_write_8bit(0x03, 0x05, 0x01);
	  break;
	case M4MO_ISO_100:
	  m4mo_write_8bit(0x03, 0x05, 0x02);
	  break;
	case M4MO_ISO_200:
	  m4mo_write_8bit(0x03, 0x05, 0x03);
	  break;
	case M4MO_ISO_400:
	  m4mo_write_8bit(0x03, 0x05, 0x04);
	  break;
	case M4MO_ISO_800:
	  m4mo_write_8bit(0x03, 0x05, 0x05);
	  break;
	case M4MO_ISO_1000:
	  m4mo_write_8bit(0x03, 0x05, 0x06);
	  break;
	default:
	    LOGE("Invalid iso value") ;
      }
      mIso = value ;
    }else {
      LOGE("iso %s not found", mParameters.get("iso") ) ;
    }
}

void QualcommCameraHardware::setEffect()
{
    int32_t value = getParm("effect", effect);

    // same as before, do nothing
    if( value == mEffect ) 
      return ;
    
    if (value != NOT_FOUND) {
      
      LOGD("setEffect() value = %d", value ) ; 
      
      LOGD("camera effect off ==  %d", CAMERA_EFFECT_OFF ) ;
      
      unsigned char readval = m4mo_read_8bit( 0x02, 0x0B ) ;
      LOGD("readval == 0x%x", readval ) ;

      if( readval ) {
	  LOGD("reset 0x02 0x0b to 0 ");
	m4mo_write_8bit(0x02, 0x0B, 0x00); 
      }
      

      
      unsigned char mode = m4mo_read_8bit( 0x00, 0x0B ) ;
      LOGD("mode == 0x%x", mode ) ;
      
      if( mode == 0x03 ) return ;
      
      m4mo_switch_to_param_mode() ;
      
      readval = m4mo_read_8bit( 0x01, 0x0B ) ;
      LOGD("readval == 0x%x", readval ) ;

      if( readval ) {
	  LOGD("reset 0x01 0x0b to 0 ");
	m4mo_write_8bit(0x01, 0x0B, 0x00); 
      }
      
      switch( value )  {
	case CAMERA_EFFECT_OFF :
	  m4mo_write_8bit(0x02, 0x0B, 0x00); 
	  break; 
	case CAMERA_EFFECT_MONO :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x00);
	  m4mo_write_8bit(0x02, 0x0A, 0x00);
	  break; 
	case CAMERA_EFFECT_NEGATIVE :
	  m4mo_write_8bit(0x01, 0x0B, 0x01); 	
	  break; 	
	case CAMERA_EFFECT_SOLARIZE :
	  m4mo_write_8bit(0x01, 0x0B, 0x02); 
	  break;
	case CAMERA_EFFECT_SEPIA :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0xD8);
	  m4mo_write_8bit(0x02, 0x0A, 0x18);
	  break; 			
	case CAMERA_EFFECT_AQUA :
	  m4mo_write_8bit(0x01, 0x0B, 0x08); 
	  break; 			
	case CAMERA_EFFECT_BLUE :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x40);
	  m4mo_write_8bit(0x02, 0x0A, 0x00);
	  break; 
	case CAMERA_EFFECT_GREEN :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0xE0);
	  m4mo_write_8bit(0x02, 0x0A, 0xE0);
	  break; 	  
	case CAMERA_EFFECT_RED :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x00);
	  m4mo_write_8bit(0x02, 0x0A, 0x6B);
	  break; 	
	case CAMERA_EFFECT_PINK :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x20);
	  m4mo_write_8bit(0x02, 0x0A, 0x40);
	  break; 	
	case CAMERA_EFFECT_YELLOW :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x80);
	  m4mo_write_8bit(0x02, 0x0A, 0x15);
	  break; 		  
	case CAMERA_EFFECT_PURPLE :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0x50);
	  m4mo_write_8bit(0x02, 0x0A, 0x20);
	  break; 
	case CAMERA_EFFECT_ANTIQUE :
	  m4mo_write_8bit(0x02, 0x0B, 0x01); 
	  m4mo_write_8bit(0x02, 0x09, 0xD0);
	  m4mo_write_8bit(0x02, 0x0A, 0x30);
	  break; 
	case CAMERA_EFFECT_SOLARIZE2 :
	  m4mo_write_8bit(0x01, 0x0B, 0x03); 
	  break; 
	case CAMERA_EFFECT_SOLARIZE3 :
	  m4mo_write_8bit(0x01, 0x0B, 0x04); 
	  break; 
	case CAMERA_EFFECT_SOLARIZE4 :
	  m4mo_write_8bit(0x01, 0x0B, 0x05); 
	  break; 
	case CAMERA_EFFECT_EMBOSS :
	  m4mo_write_8bit(0x01, 0x0B, 0x06); 
	  break; 
	case CAMERA_EFFECT_OUTLINE :
	  m4mo_write_8bit(0x01, 0x0B, 0x07); 
	  break; 	          
      }
      m4mo_switch_to_monitor_mode() ;
      mEffect = value ;
    }
}

void QualcommCameraHardware::setZoom()
{
    int32_t val = mParameters.getInt("zoom") ;

    // same as before, do nothing
    if( val == mZoom ) 
      return ;
    
    unsigned char current = m4mo_read_8bit(0x02, 0x01);
        LOGD("CURRENT ZOOM = 0%x", current ) ;

    unsigned char value = 0xA + ( val * 3 ) ;
    	if((value < 0xA) || (value > 0xFA))
	{
		LOGE("Error, Zoom Value is out of range!.");
		return;
	}
    	m4mo_write_8bit(0x02, 0x01, value); 
	//mdelay(30);
	usleep( 30000 ) ;
    mZoom = val ;
}

void QualcommCameraHardware::setWhiteBalance()
{
    int32_t value = getParm("whitebalance", whitebalance);
    
    // same as before, do nothing
    if( value == mWhiteBalance ) 
      return ;
    
    if (value != NOT_FOUND) {
      switch( value ) {
	case CAMERA_WB_AUTO :
	  m4mo_write_8bit(0x06, 0x02, 0x01);
	  break ;
	case CAMERA_WB_INCANDESCENT :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x01);
	  break ;
	case CAMERA_WB_FLUORESCENT :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x02);
	  break ;
	case CAMERA_WB_DAYLIGHT :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x03);
	  break ;	  
	case CAMERA_WB_CLOUDY_DAYLIGHT :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x04);
	  break ;	
	case CAMERA_WB_SHADE :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x05);
	  break ;	  
	case CAMERA_WB_HORIZON :
	  m4mo_write_8bit(0x06, 0x02, 0x02);
	  m4mo_write_8bit(0x06, 0x03, 0x06);
	  break ;	  	  
      }
      mWhiteBalance = value ;
    }
}

void QualcommCameraHardware::setSceneMode()
{
  int32_t value = getParm("scene-mode", scenemode);
  // same as before, do nothing
    if( value == mSceneMode ) 
      return ;
    if (value != NOT_FOUND) {
      switch( value ) {
	case M4MO_SCENE_AUTO :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x00);
	  m4mo_write_8bit(0x03, 0x0B, 0x00);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "7.5") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "15.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;
	  break;
	case M4MO_SCENE_PORTRAIT :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "on") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x01);
	  m4mo_write_8bit(0x03, 0x0B, 0x01);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "7.5") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "5.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;	  
	  break;	
	case M4MO_SCENE_LANDSCAPE :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-average") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x02);
	  m4mo_write_8bit(0x03, 0x0B, 0x02);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "7.5") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "15.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;		  
	  break;		  
	case M4MO_SCENE_BEACH_SNOW :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x05);
	  m4mo_write_8bit(0x03, 0x0B, 0x05);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x04);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "7.5") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "10.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;		  
	  break;	  
	case M4MO_SCENE_SUNSET :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x06);
	  m4mo_write_8bit(0x03, 0x0B, 0x06);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "5.0") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "10.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;	
	  break;
	case M4MO_SCENE_NIGHT :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x09);
	  m4mo_write_8bit(0x03, 0x0B, 0x09);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "5.0") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "10.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "auto") ;
	  setIso() ;	
	  break;	  	  	  
	case M4MO_SCENE_FIREWORK :
	  mParameters.set("focus-mode", "normal") ;
	  setFocusMode() ;
	 
	  mParameters.set("face-detection", "off") ;
	  setFaceDetection() ;
	  
	  mParameters.set("meter-mode", "meter-center") ;
	  setAutoExposure() ;
	  
	  m4mo_write_8bit(0x03, 0x0A, 0x0A);
	  m4mo_write_8bit(0x03, 0x0B, 0x0A);
	  m4mo_write_8bit(0x03, 0x02, 0x13);
	  m4mo_write_8bit(0x03, 0x09, 0x03);
	  
	  mParameters.set("whitebalance", "auto") ;
	  setWhiteBalance() ;
	  mParameters.set("saturation", "5.0") ;
	  setSaturation() ;
	  mParameters.set("sharpness", "10.0") ;	 
	  setSharpness() ;
	  m4mo_write_8bit(0x03, 0x05, 0x00);
	  mParameters.set("iso", "50") ;
	  setIso() ;	
	  break;	  	  	  	  
      }
    }
    mSceneMode = value ;
}
void QualcommCameraHardware::setAntibanding()
{
    camera_antibanding_type value =
        (camera_antibanding_type) getParm("antibanding", antibanding);
    native_set_parm(CAMERA_SET_PARM_ANTIBANDING, sizeof(value), (void *)&value);
}

bool QualcommCameraHardware::flashNeeded()
{
  const char *str = mParameters.get("flash-mode");
  LOGD("get-flashmode %s", str ) ;
  if( strcmp( "auto", str) == 0 ) {
    LOGD("flash-mode == auto") ;
    unsigned char val1 = m4mo_read_8bit( 0x03, 0x1e ) ;
    unsigned char val2 = m4mo_read_8bit( 0x03, 0x1f ) ;

    int res = (val1*256) + val2 ;

    LOGD("Flash needed Value found : %d ( 0x%x, 0x%x )", res, val1, val2 ) ;
    return res < 1150 ;
  } else if( strcmp( "on", str ) == 0 ) {
    LOGD("flash-mode == on") ;
    return true ;
  } else {
    LOGD("flash-mode == off") ;
    return false; 
  }
}

void QualcommCameraHardware::startFlashMovie()
{
  ioctl_msg_info ctrl_info;
  ctrl_info.codeA = FLASH_CMD ;
  ctrl_info.codeB = FLASH_MOVIE ; 
  ctrl_info.codeC = FLASH_CMD_ON ;
  if ((ioctl( mCameraControlFd, MSM_CAM_IOCTL_PGH_MSG, &ctrl_info)) < 0)
    LOGE("MSM_CAM_IOCTL_PGH_COMMAND : ioctl fd %d error %s\n",
      mCameraControlFd,
      strerror(errno));
}

void QualcommCameraHardware::stopFlashMovie()
{
  ioctl_msg_info ctrl_info;
  ctrl_info.codeA = FLASH_CMD ;
  ctrl_info.codeB = FLASH_MOVIE ; 
  ctrl_info.codeC = FLASH_CMD_OFF ;
  if ((ioctl( mCameraControlFd, MSM_CAM_IOCTL_PGH_MSG, &ctrl_info)) < 0)
    LOGE("MSM_CAM_IOCTL_PGH_COMMAND : ioctl fd %d error %s\n",
      mCameraControlFd,
      strerror(errno));
}


void QualcommCameraHardware::startFlash()
{
  ioctl_msg_info ctrl_info;
  ctrl_info.codeA = FLASH_CMD ;
  ctrl_info.codeB = FLASH_CAMERA ; 
  ctrl_info.codeC = FLASH_CMD_ON ;
  if ((ioctl( mCameraControlFd, MSM_CAM_IOCTL_PGH_MSG, &ctrl_info)) < 0)
    LOGE("MSM_CAM_IOCTL_PGH_COMMAND : ioctl fd %d error %s\n",
      mCameraControlFd,
      strerror(errno));
}

void QualcommCameraHardware::stopFlash()
{
  ioctl_msg_info ctrl_info;
  ctrl_info.codeA = FLASH_CMD ;
  ctrl_info.codeB = FLASH_CAMERA ; 
  ctrl_info.codeC = FLASH_CMD_OFF ;
  if ((ioctl( mCameraControlFd, MSM_CAM_IOCTL_PGH_MSG, &ctrl_info)) < 0)
    LOGE("MSM_CAM_IOCTL_PGH_COMMAND : ioctl fd %d error %s\n",
      mCameraControlFd,
      strerror(errno));
}

void QualcommCameraHardware::setLensToBasePosition()
{
  int32_t value = getParm("focus-mode", focusmode);
  if( value == M4MO_AF_MACRO ) {
    m4mo_write_8bit( 0x0a, 0x10, 0x02 ) ;
  } else {
    m4mo_write_8bit( 0x0a, 0x10, 0x01 ) ;
  }
  
// Give it some time ( like donut kernel logs )
 // usleep(200*1000);	  
  
  if( m4mo_wait_for_value(0x0A, 0x10, 0x00, 50) ) {
    LOGD("Wait focus stop OK") ;
  } else {
    LOGE("Failed to wait a AF stop!") ;
  }
 
  if( value == M4MO_AF_NORMAL ) {
    m4mo_write_8bit( 0x0a, 0x00, 0x00 ) ;
  }
  
}

QualcommCameraHardware::MemPool::MemPool(int buffer_size, int num_buffers,
                                         int frame_size,
                                         int frame_offset,
                                         const char *name) :
    mBufferSize(buffer_size),
    mNumBuffers(num_buffers),
    mFrameSize(frame_size),
    mFrameOffset(frame_offset),
    mBuffers(NULL), mName(name)
{
    // empty
}

void QualcommCameraHardware::MemPool::completeInitialization()
{
    // If we do not know how big the frame will be, we wait to allocate
    // the buffers describing the individual frames until we do know their
    // size.
    LOGD("mBufferSize = %d, mFrameOffset = %d, mFrameSize = %d", mBufferSize, mFrameOffset, mFrameSize ) ;
    if (mFrameSize > 0) {
        mBuffers = new sp<MemoryBase>[mNumBuffers];
        for (int i = 0; i < mNumBuffers; i++) {
            mBuffers[i] = new
                MemoryBase(mHeap,
                           i * mBufferSize + mFrameOffset,
                           mFrameSize);
        }
    }
}

 void QualcommCameraHardware::MemPool::completeInitializationnew()
 {
 LOGD("QualcommCameraHardware::MemPool::completeInitializationnew");

     if (mFrameSize > 0) {
         mBuffers = new sp<MemoryBase>[mNumBuffers];
         for (int i = 0; i < mNumBuffers; i++) {
 LOGI("SFbufs: i = %d mBufferSize = %d mFrameOffset = %d mFrameSize = %d\n", i, mBufferSize, mFrameOffset, mFrameSize);
             mBuffers[i] = new
                 MemoryBase(mHeapnew[i], 0, mFrameSize);
         }
     }
 }

QualcommCameraHardware::AshmemPool::AshmemPool(int buffer_size, int num_buffers,
                                               int frame_size,
                                               int frame_offset,
                                               const char *name) :
    QualcommCameraHardware::MemPool(buffer_size,
                                    num_buffers,
                                    frame_size,
                                    frame_offset,
                                    name)
{
    LOGD("constructing MemPool %s backed by ashmem: "
         "%d frames @ %d uint8_ts, offset %d, "
         "buffer size %d",
         mName,
         num_buffers, frame_size, frame_offset, buffer_size);

    int page_mask = getpagesize() - 1;
    int ashmem_size = buffer_size * num_buffers;
    ashmem_size += page_mask;
    ashmem_size &= ~page_mask;

    mHeap = new MemoryHeapBase(ashmem_size);

    completeInitialization();
}


QualcommCameraHardware::PmemPool::PmemPool(const char *pmem_pool,
                                           int camera_control_fd,
                                           msm_pmem_t pmem_type,
                                           int buffer_size, int num_buffers,
                                           int frame_size,
                                           int frame_offset,
                                           const char *name) :
    QualcommCameraHardware::MemPool(buffer_size,
                                    num_buffers,
                                    frame_size,
                                    frame_offset,
                                    name),
    mPmemType(pmem_type),
    mCameraControlFd(camera_control_fd)
{
  LOGV("constructing MemPool %s backed by pmem pool %s: "
              "%d frames @ %d bytes, offset %d, buffer size %d",
              mName,
              pmem_pool, num_buffers, frame_size, frame_offset,
              buffer_size);

         ptypeflag = 0;
         
         // Make a new mmap'ed heap that can be shared across processes.
         
         mAlignedSize = clp2(buffer_size * num_buffers);
         
         sp<MemoryHeapBase> masterHeap = 
             new MemoryHeapBase(pmem_pool, mAlignedSize, 0);
         sp<MemoryHeapPmem> pmemHeap = new MemoryHeapPmem(masterHeap, 0);
         if (pmemHeap->getHeapID() >= 0) {
             pmemHeap->slap();
             masterHeap.clear();
             mHeap = pmemHeap;
             pmemHeap.clear();
             
             mFd = mHeap->getHeapID();
             if (::ioctl(mFd, PMEM_GET_SIZE, &mSize)) {
                 LOGE("pmem pool %s ioctl(PMEM_GET_SIZE) error %s (%d)",
                      pmem_pool,
                      ::strerror(errno), errno);
                 mHeap.clear();
                 return;
             }
             
             LOGE("pmem pool %s ioctl(PMEM_GET_SIZE) is %ld",
                  pmem_pool,
                  mSize.len);
             
             completeInitialization();
	 }
}


QualcommCameraHardware::PmemPool::PmemPool(const char *pmem_pool,
                                           int camera_control_fd,
                                           msm_pmem_t pmem_type,
                                           int buffer_size, int num_buffers,
                                           int frame_size,
                                           int frame_offset,
                                           const char *name, int flag) :
    QualcommCameraHardware::MemPool(buffer_size,
                                    num_buffers,
                                    frame_size,
                                    frame_offset,
                                    name),
    mPmemType(pmem_type),
    mCameraControlFd(camera_control_fd)
{
    LOGD("constructing MemPool %s backed by pmem pool %s: "
         "%d frames @ %d bytes, offset %d, buffer size %d",
         mName,
         pmem_pool, num_buffers, frame_size, frame_offset,
         buffer_size);

/*
    if( mCameraControlFd == 0 ) {
    	LOGD("duped FD is 0 , dup again ") ;
	mCameraControlFd = dup(camera_control_fd);
    }
    LOGD("%s: duplicating control fd %d --> %d",
         __FUNCTION__,
         camera_control_fd, mCameraControlFd);
*/
    // Make a new mmap'ed heap that can be shared across processes.

	  sp<MemoryHeapBase> masterHeap;
         sp<MemoryHeapPmem> pmemHeap;

         ptypeflag = 0;
         
         buffer_size = clp2(buffer_size);
         for (int i = 0; i < num_buffers; i++) {
             masterHeap = new MemoryHeapBase(pmem_pool, buffer_size, 0);
             pmemHeap = new MemoryHeapPmem(masterHeap, 0);
 LOGE("pmemheap: id = %d base = %x", (int)pmemHeap->getHeapID(), (unsigned int)pmemHeap->base());
             if (pmemHeap->getHeapID() >= 0) {
                 pmemHeap->slap();
                 masterHeap.clear();
                 mHeapnew[i] = pmemHeap;
                 pmemHeap.clear();
                 
                 mFd = mHeapnew[i]->getHeapID();
                 if (::ioctl(mFd, PMEM_GET_SIZE, &mSize)) {
                     LOGE("pmem pool %s ioctl(PMEM_GET_SIZE) error %s (%d)",
                      pmem_pool,
                      ::strerror(errno), errno);
                     mHeapnew[i].clear();
                     return;
                 }
                 
                 LOGE("pmem pool %s ioctl(PMEM_GET_SIZE) is %ld",
                      pmem_pool,
                      mSize.len);
             }
             else {
                 LOGE("pmem pool %s error: could not create master heap!", pmem_pool);
             }
         }
         completeInitializationnew();

        // Register preview buffers with the camera drivers.
        /*for (int cnt = 0; cnt < num_buffers; ++cnt) {
            register_buf(mCameraControlFd,
                         buffer_size,
                         mHeap->getHeapID(),
                         buffer_size * cnt,
                         (uint8_t *)mHeap->base() + buffer_size * cnt,			 
                         pmem_type, cnt == ( num_buffers - 1 ) ? false : true );
        }*/
}

QualcommCameraHardware::PmemPool::~PmemPool()
{
    LOGD("%s: %s E", __FUNCTION__, mName);
    // Unregister preview buffers with the camera drivers.
    /*for (int cnt = 0; cnt < mNumBuffers; ++cnt) {
        register_buf(mCameraControlFd,
                     mBufferSize,
                     mHeap->getHeapID(),
                     mBufferSize * cnt,
                     (uint8_t *)mHeap->base() + mBufferSize * cnt,
                     mPmemType,
                     true, false);
    }*/
    LOGD("destroying PmemPool %s: ", 
         mName);
    //close(mCameraControlFd);
    LOGD("%s: %s X", __FUNCTION__, mName);
}

QualcommCameraHardware::MemPool::~MemPool()
{
    LOGD("destroying MemPool %s", mName);
    if (mFrameSize > 0)
        delete [] mBuffers;
    mHeap.clear();
    LOGD("destroying MemPool %s completed", mName);
}

     QualcommCameraHardware::PreviewPmemPool::PreviewPmemPool(
	      int control_fd,
             int buffer_size, int num_buffers,
             int frame_size,
             int frame_offset,
             const char *name,
             int flag) :
         QualcommCameraHardware::PmemPool("/dev/pmem_adsp",
					  control_fd,MSM_PMEM_OUTPUT2,
                                          buffer_size,
                                          num_buffers,
                                         frame_size,
                                          frame_offset,
                                          name, 1)
     {
         LOGV("constructing PreviewPmemPool");
         if (initialized()) {
             //NOTE : SOME PREVIEWPMEMPOOL SPECIFIC CODE MAY BE ADDED
         }
     }
     QualcommCameraHardware::PreviewPmemPool::PreviewPmemPool(
	    int control_fd, 
            int buffer_size, int num_buffers,
             int frame_size,
             int frame_offset,
             const char *name) :
         QualcommCameraHardware::PmemPool("/dev/pmem_adsp",control_fd,MSM_PMEM_OUTPUT2,
                                          buffer_size,
                                          num_buffers,
                                          frame_size,
                                          frame_offset,
                                          name,1)
     {
 LOGD("QualcommCameraHardware::PreviewPmemPool::PreviewPmemPool");
         LOGV("constructing PreviewPmemPool");
         if (initialized()) {
             //NOTE : SOME PREVIEWPMEMPOOL SPECIFIC CODE MAY BE ADDED
         }
     }

     QualcommCameraHardware::PreviewPmemPool::~PreviewPmemPool()
     {
         LOGV("destroying PreviewPmemPool");
         if(initialized()) {
                         LOGV("destroying PreviewPmemPool");
         }
     }

static bool register_buf(int camfd,
                         int size,
                         int pmempreviewfd,
                         uint32_t offset,
                         uint8_t *buf,
                         msm_pmem_t pmem_type,
			 bool active ,
                         bool register_buffer)
{
    struct msm_pmem_info_t pmemBuf;

    pmemBuf.type     = pmem_type;
    pmemBuf.fd       = pmempreviewfd;
    //pmemBuf.offset   = offset;
    //pmemBuf.len      = size;
    pmemBuf.vaddr    = buf ;
    pmemBuf.y_off    = 0;
    //pmemBuf.cbcr_off = size * 2 / 3; //PAD_TO_WORD(size * 2 / 3);
    //pmemBuf.vfe_can_write   = true;
    pmemBuf.active   = active ;

    if( pmem_type == MSM_PMEM_RAW_MAINIMG ) 
    	pmemBuf.cbcr_off = 0;
    else 
    	pmemBuf.cbcr_off = ((size * 2 / 3)+1) & ~1 ;


    /*LOGD("register_buf: camfd = %d, pmemfd = %d, reg = %d buffer = %p offset = %d, size = %d, cbrc_off = %d",
         camfd, pmempreviewfd, !register_buffer, buf, offset, size, pmemBuf.cbcr_off );*/
    if (ioctl(camfd,
              register_buffer ?
              MSM_CAM_IOCTL_REGISTER_PMEM :
              MSM_CAM_IOCTL_UNREGISTER_PMEM,
              &pmemBuf) < 0) {
        LOGE("register_buf: MSM_CAM_IOCTL_(UN)REGISTER_PMEM fd %d error %s",
             camfd,
             strerror(errno));
        return false;
    }
    return true;
}

status_t QualcommCameraHardware::MemPool::dump(int fd, const Vector<String16>& args) const
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    snprintf(buffer, 255, "QualcommCameraHardware::AshmemPool::dump\n");
    result.append(buffer);
    if (mName) {
        snprintf(buffer, 255, "mem pool name (%s)\n", mName);
        result.append(buffer);
    }
    if (mHeap != 0) {
        snprintf(buffer, 255, "heap base(%p), size(%d), flags(%d), device(%s)\n",
                 mHeap->getBase(), mHeap->getSize(),
                 mHeap->getFlags(), mHeap->getDevice());
        result.append(buffer);
    }
    snprintf(buffer, 255, "buffer size (%d), number of buffers (%d),"
             " frame size(%d), and frame offset(%d)\n",
             mBufferSize, mNumBuffers, mFrameSize, mFrameOffset);
    result.append(buffer);
    write(fd, result.string(), result.size());
    return NO_ERROR;
}

static void receive_camframe_callback(struct msm_frame_t *frame)
{
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->receivePreviewFrame(frame);
    }
}

static void receive_jpeg_fragment_callback(uint8_t *buff_ptr, uint32_t buff_size)
{
    LOGD("receive_jpeg_fragment_callback E");
    sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
    if (obj != 0) {
        obj->receiveJpegPictureFragment(buff_ptr, buff_size);
    }
    LOGD("receive_jpeg_fragment_callback X");
}

static void receive_jpeg_callback(jpeg_event_t status)
{
    LOGD("receive_jpeg_callback E (completion status %d)", status);
    if (status == JPEG_EVENT_DONE) {
        sp<QualcommCameraHardware> obj = QualcommCameraHardware::getInstance();
        if (obj != 0) {
            obj->receiveJpegPicture();
        }
    }
    LOGD("receive_jpeg_callback X");
}

status_t QualcommCameraHardware::sendCommand(int32_t command, int32_t arg1,
                                             int32_t arg2)
{
    LOGD("sendCommand: EX");
    return BAD_VALUE;
}




}; // namespace android
     static void dump_to_file(const char *fname,
                              uint8_t *buf, uint32_t size)
     {
         int nw, cnt = 0;
         uint32_t written = 0;

         LOGD("opening file [%s]\n", fname);
         int fd = open(fname, O_RDWR | O_CREAT);
         if (fd < 0) {
             LOGE("failed to create file [%s]: %s", fname, strerror(errno));
             return;
         }

         LOGD("writing %d bytes to file [%s]\n", size, fname);
         while (written < size) {
             nw = ::write(fd,
                          buf + written,
                          size - written);
             if (nw < 0) {
                 LOGE("failed to write to file [%s]: %s",
                      fname, strerror(errno));
                 break;
             }
             written += nw;
             cnt++;
         }
         LOGD("done writing %d bytes to file [%s] in %d passes\n",
              size, fname, cnt);
         ::close(fd);
     }


