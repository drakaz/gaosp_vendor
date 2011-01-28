LOCAL_PATH:= $(call my-dir)

include $(CLEAR_VARS)

LOCAL_SRC_FILES:= QualcommCameraHardware.cpp exifwriter.c

LOCAL_C_INCLUDES+= \
	vendor/qcom/proprietary/mm-camera/common \
	vendor/qcom/proprietary/mm-camera/apps/appslib \
	external/jhead \
	vendor/qcom/proprietary/mm-camera/jpeg/inc

LOCAL_SHARED_LIBRARIES:= libbinder libutils libcamera_client liblog libdl libexif

LOCAL_CFLAGS += -DUSE_CUPCAKE_CHOCOLATE_CAMERA

LOCAL_MODULE:= libcamera
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

