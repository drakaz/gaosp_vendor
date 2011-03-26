/*
** Copyright 2008, The Android Open-Source Project
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

#include <math.h>

#define LOG_NDEBUG 1
#define LOG_TAG "AudioHardwareMSM72XX"
#include <utils/Log.h>
#include <utils/String8.h>

#include <stdio.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dlfcn.h>
#include <fcntl.h>

// hardware specific functions

#include "AudioHardware.h"
#include <media/AudioRecord.h>

extern "C" {
#include "max9877.h" /* define ioctls */
}

#define LOG_SND_RPC 1  // Set to 1 to log sound RPC's

#define MAX9877_SPEAKER 0
#define MAX9877_HEADSET 2
#define MAX9877_RCV 12
#define MAX9877_SPKPHONE 26

namespace android {
static int audpre_index, tx_iir_index;
static void * acoustic;
const uint32_t AudioHardware::inputSamplingRates[] = {
        8000, 11025, 12000, 16000, 22050, 24000, 32000, 44100, 48000
};
static uint16_t _ADRC[8] = {9728,45875,262,32637,12438,32759,17238,22};
static uint16_t _IIR[49] = {4,0,16384,24628,17292,941,3611,14425,15821,62243,54449,50698,3581,14425,15821,49700,46507,60894,12499,0,16384,65079,48250,51635,13098,
				12602,17754,63371,3945,47023,54054,7720,3477,52359,45148,49850,12552,45531,48995,17201,13037,2,2,2,2,0,0,0,0};

static uint16_t _EQ[139] = {8,9612,15792,20419,35120,17259,14665,29320,16752,61078,33576,9519,15275,54189,16947,25371,36513,46803,13086,
				10026,17382,62792,41189,49637,10546,12668,15293,13316,45132,59943,10004,65532,16383,25812,51072,
				38302,9005,35400,17980,22873,53402,23776,7047,28065,1360,12483,1462,33741,7640,32697,35182,39171,14135,
				61078,33576,38913,15643,25371,36513,35465,13650,62792,41189,59664,11544,13316,45132,7073,8914,25812,51072,
				38300,9005,22873,53402,59176,8643,26830,55761,47476,3853,2,2,2,2,2,2,2,2,0,0,0,
				0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

static status_t set_volume_rpc(uint32_t device,
                               uint32_t method,
                               uint32_t volume);

// ----------------------------------------------------------------------------

static int do_filter_rpc(uint32_t filter, void *args) {
    int fd;
    fd = open("/dev/msm_pcm_ctl", O_RDWR);
    if (fd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
     if (ioctl(fd, filter, args) < 0) {
         LOGE("do_filter_rpc error filter:%d", filter);
         close(fd);
         return -EIO;
     }
     close(fd);
     return NO_ERROR;

}

static int do_audpp_enable_rpc(uint16_t flags) {
    int fd;
    fd = open("/dev/msm_pcm_ctl", O_RDWR);
    if (fd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
     if (ioctl(fd, AUDIO_ENABLE_AUDPP, &flags) < 0) {
         LOGE("do_audpp_enable_rpc error");
         close(fd);
         return -EIO;
     }
     close(fd);
     return NO_ERROR;
}

static void MAX9877_ioctl_data(int cmd, void *data) {
     static int max_fd = -1;
     if(max_fd == -1) {
         max_fd = open("/dev/max9877", O_RDWR);
         if(max_fd == -1) {
               LOGE("-- cant open max9877\n");
               return;
         }
     }
     if(ioctl(max_fd, cmd, data) < 0){
         LOGE("-- cant ioctl max9877\n");
     }
}

static void MAX9877_ioctl(int cmd) {
     MAX9877_ioctl_data(cmd, NULL);
}


AudioHardware::AudioHardware() :
    mInit(false), mMicMute(true), mBluetoothNrec(true), mBluetoothId(0),
    mOutput(0), mSndEndpoints(NULL), mCurSndDevice(-1),
    SND_DEVICE_CURRENT(28),
    SND_DEVICE_HANDSET(0),
    SND_DEVICE_SPEAKER(6),
    SND_DEVICE_SPEAKER_MIDI(26),
    SND_DEVICE_HEADSET(3),
    SND_DEVICE_HEADSET_INCALL(2),
    SND_DEVICE_BT(12),
    SND_DEVICE_CARKIT(-1),
    SND_DEVICE_TTY_FULL(-1),
    SND_DEVICE_TTY_VCO(-1),
    SND_DEVICE_TTY_HCO(-1),
    SND_DEVICE_NO_MIC_HEADSET(-1),
    SND_DEVICE_FM_HEADSET(-1),
    SND_DEVICE_HEADSET_AND_SPEAKER(-1),
    SND_DEVICE_FM_SPEAKER(-1),
    SND_DEVICE_BT_EC_OFF(-1)
{
    do_filter_rpc(AUDIO_SET_ADRC, &_ADRC);
    do_filter_rpc(AUDIO_SET_RX_IIR, &_IIR);
    do_filter_rpc(AUDIO_SET_EQ, &_EQ);

    set_volume_rpc(SND_DEVICE_HANDSET, SND_METHOD_VOICE, 5);
    set_volume_rpc(SND_DEVICE_SPEAKER, SND_METHOD_VOICE, 5);
    set_volume_rpc(SND_DEVICE_BT, SND_METHOD_VOICE, 5);
    set_volume_rpc(SND_DEVICE_HEADSET,SND_METHOD_VOICE, 5);

    mNumSndEndpoints = 0;
    mInit = true;
    
}

AudioHardware::~AudioHardware()
{
    for (size_t index = 0; index < mInputs.size(); index++) {
        closeInputStream((AudioStreamIn*)mInputs[index]);
    }
    mInputs.clear();
    closeOutputStream((AudioStreamOut*)mOutput);
    delete [] mSndEndpoints;
    if (acoustic) {
        ::dlclose(acoustic);
        acoustic = 0;
    }
    mInit = false;
}

status_t AudioHardware::initCheck()
{
    return mInit ? NO_ERROR : NO_INIT;
}

AudioStreamOut* AudioHardware::openOutputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status)
{
    { // scope for the lock
        Mutex::Autolock lock(mLock);

        // only one output stream allowed
        if (mOutput) {
            if (status) {
                *status = INVALID_OPERATION;
            }
            return 0;
        }

        // create new output stream
        AudioStreamOutMSM72xx* out = new AudioStreamOutMSM72xx();
        status_t lStatus = out->set(this, devices, format, channels, sampleRate);
        if (status) {
            *status = lStatus;
        }
        if (lStatus == NO_ERROR) {
            mOutput = out;
        } else {
            delete out;
        }
    }
    return mOutput;
}

void AudioHardware::closeOutputStream(AudioStreamOut* out) {
    Mutex::Autolock lock(mLock);
    if (mOutput == 0 || mOutput != out) {
        LOGW("Attempt to close invalid output stream");
    }
    else {
        delete mOutput;
        mOutput = 0;
    }
}

AudioStreamIn* AudioHardware::openInputStream(
        uint32_t devices, int *format, uint32_t *channels, uint32_t *sampleRate, status_t *status,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    // check for valid input source
    if (!AudioSystem::isInputDevice((AudioSystem::audio_devices)devices)) {
        return 0;
    }

    mLock.lock();

    AudioStreamInMSM72xx* in = new AudioStreamInMSM72xx();
    status_t lStatus = in->set(this, devices, format, channels, sampleRate, acoustic_flags);
    if (status) {
        *status = lStatus;
    }
    if (lStatus != NO_ERROR) {
        mLock.unlock();
        delete in;
        return 0;
    }

    mInputs.add(in);
    mLock.unlock();

    return in;
}

void AudioHardware::closeInputStream(AudioStreamIn* in) {
    Mutex::Autolock lock(mLock);

    ssize_t index = mInputs.indexOf((AudioStreamInMSM72xx *)in);
    if (index < 0) {
        LOGW("Attempt to close invalid input stream");
    } else {
        mLock.unlock();
        delete mInputs[index];
        mLock.lock();
        mInputs.removeAt(index);
    }
}

status_t AudioHardware::setMode(int mode)
{
    status_t status = AudioHardwareBase::setMode(mode);
    if (status == NO_ERROR) {
        // make sure that doAudioRouteOrMute() is called by doRouting()
        // even if the new device selected is the same as current one.
        clearCurDevice();
    }
    return status;
}

bool AudioHardware::checkOutputStandby()
{
    if (mOutput)
        if (!mOutput->checkStandby())
            return false;

    return true;
}

status_t AudioHardware::setMicMute(bool state)
{
    Mutex::Autolock lock(mLock);
    return setMicMute_nosync(state);
}

// always call with mutex held
status_t AudioHardware::setMicMute_nosync(bool state)
{
    if (mMicMute != state) {
        mMicMute = state;
        return doAudioRouteOrMute(SND_DEVICE_CURRENT);
    }
    return NO_ERROR;
}

status_t AudioHardware::getMicMute(bool* state)
{
    *state = mMicMute;
    return NO_ERROR;
}

status_t AudioHardware::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 value;
    String8 key;
    const char BT_NREC_KEY[] = "bt_headset_nrec";
    const char BT_NAME_KEY[] = "bt_headset_name";
    const char BT_NREC_VALUE_ON[] = "on";


    LOGV("setParameters() %s", keyValuePairs.string());

    if (keyValuePairs.length() == 0) return BAD_VALUE;

    key = String8(BT_NREC_KEY);
    if (param.get(key, value) == NO_ERROR) {
        if (value == BT_NREC_VALUE_ON) {
            mBluetoothNrec = true;
        } else {
            mBluetoothNrec = false;
            LOGI("Turning noise reduction and echo cancellation off for BT "
                 "headset");
        }
    }
    key = String8(BT_NAME_KEY);
    if (param.get(key, value) == NO_ERROR) {
        mBluetoothId = 0;
        for (int i = 0; i < mNumSndEndpoints; i++) {
            if (!strcasecmp(value.string(), mSndEndpoints[i].name)) {
                mBluetoothId = mSndEndpoints[i].id;
                LOGI("Using custom acoustic parameters for %s", value.string());
                break;
            }
        }
        if (mBluetoothId == 0) {
            LOGI("Using default acoustic parameters "
                 "(%s not in acoustic database)", value.string());
            doRouting();
        }
    }
    return NO_ERROR;
}

String8 AudioHardware::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    return param.toString();
}


static unsigned calculate_audpre_table_index(unsigned index)
{
    switch (index) {
        case 48000:    return SAMP_RATE_INDX_48000;
        case 44100:    return SAMP_RATE_INDX_44100;
        case 32000:    return SAMP_RATE_INDX_32000;
        case 24000:    return SAMP_RATE_INDX_24000;
        case 22050:    return SAMP_RATE_INDX_22050;
        case 16000:    return SAMP_RATE_INDX_16000;
        case 12000:    return SAMP_RATE_INDX_12000;
        case 11025:    return SAMP_RATE_INDX_11025;
        case 8000:    return SAMP_RATE_INDX_8000;
        default:     return -1;
    }
}
size_t AudioHardware::getInputBufferSize(uint32_t sampleRate, int format, int channelCount)
{
    if (format != AudioSystem::PCM_16_BIT) {
        LOGW("getInputBufferSize bad format: %d", format);
        return 0;
    }
    if (channelCount < 1 || channelCount > 2) {
        LOGW("getInputBufferSize bad channel count: %d", channelCount);
        return 0;
    }

    return 2048*channelCount;
}

static status_t set_volume_rpc(uint32_t device,
                               uint32_t method,
                               uint32_t volume)
{
    int fd;
#if LOG_SND_RPC
    LOGD("rpc_snd_set_volume(%d, %d, %d)\n", device, method, volume);
#endif

    if (device == -1UL) return NO_ERROR;

    fd = open("/dev/msm_snd", O_RDWR);
    if (fd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
    /* rpc_snd_set_volume(
     *     device,            # Any hardware device enum, including
     *                        # SND_DEVICE_CURRENT
     *     method,            # must be SND_METHOD_VOICE to do anything useful
     *     volume,            # integer volume level, in range [0,5].
     *                        # note that 0 is audible (not quite muted)
     *  )
     * rpc_snd_set_volume only works for in-call sound volume.
     */
     struct msm_snd_volume_config args;
     args.device = device;
     args.method = method;
     args.volume = volume;

     if (ioctl(fd, SND_SET_VOLUME, &args) < 0) {
         LOGE("snd_set_volume error.");
         close(fd);
         return -EIO;
     }
     close(fd);
     return NO_ERROR;
}

status_t AudioHardware::setVoiceVolume(float v)
{
    if (v < 0.0) {
        LOGW("setVoiceVolume(%f) under 0.0, assuming 0.0\n", v);
        v = 0.0;
    } else if (v > 1.0) {
        LOGW("setVoiceVolume(%f) over 1.0, assuming 1.0\n", v);
        v = 1.0;
    }

    int vol = lrint(v * 5.0);
    LOGD("setVoiceVolume(%f)\n", v);
    LOGI("Setting in-call volume to %d (available range is 0 to 5)\n", vol);

    Mutex::Autolock lock(mLock);
    set_volume_rpc(SND_DEVICE_CURRENT, SND_METHOD_VOICE, vol);
    return NO_ERROR;
}

status_t AudioHardware::setMasterVolume(float v)
{
    Mutex::Autolock lock(mLock);
    int vol = ceil(v * 5.0);
    LOGI("Set master volume to %d.\n", vol);
    /*
    set_volume_rpc(SND_DEVICE_HANDSET, SND_METHOD_VOICE, vol);
    set_volume_rpc(SND_DEVICE_SPEAKER, SND_METHOD_VOICE, vol);
    set_volume_rpc(SND_DEVICE_BT,      SND_METHOD_VOICE, vol);
    set_volume_rpc(SND_DEVICE_HEADSET, SND_METHOD_VOICE, vol);
    */
    // We return an error code here to let the audioflinger do in-software
    // volume on top of the maximum volume that we set through the SND API.
    // return error - software mixer will handle it
    return -1;
}

static status_t do_route_audio_rpc(uint32_t device,
                                   bool ear_mute, bool mic_mute)
{
    if (device == -1UL)
        return NO_ERROR;

    int fd;
#if LOG_SND_RPC
    LOGD("rpc_snd_set_device(%d, %d, %d)\n", device, ear_mute, mic_mute);
#endif

    fd = open("/dev/msm_snd", O_RDWR);
    if (fd < 0) {
        LOGE("Can not open snd device");
        return -EPERM;
    }
    // RPC call to switch audio path
    /* rpc_snd_set_device(
     *     device,            # Hardware device enum to use
     *     ear_mute,          # Set mute for outgoing voice audio
     *                        # this should only be unmuted when in-call
     *     mic_mute,          # Set mute for incoming voice audio
     *                        # this should only be unmuted when in-call or
     *                        # recording.
     *  )
     */
    struct msm_snd_device_config args;
    args.device = device;
    args.ear_mute = ear_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;
    args.mic_mute = mic_mute ? SND_MUTE_MUTED : SND_MUTE_UNMUTED;

    if (ioctl(fd, SND_SET_DEVICE, &args) < 0) {
        LOGE("snd_set_device error.");
        close(fd);
        return -EIO;
    }

    close(fd);
    return NO_ERROR;
}

// always call with mutex held
status_t AudioHardware::doAudioRouteOrMute(uint32_t device)
{
    if (device == (uint32_t)SND_DEVICE_BT || device == (uint32_t)SND_DEVICE_CARKIT) {
        if (mBluetoothId) {
            device = mBluetoothId;
        } else if (!mBluetoothNrec) {
            device = SND_DEVICE_BT_EC_OFF;
        }
    }

	if (LOG_NDEBUG) {
		LOGI("doAudioRouteOrMute() device %x, mMode %d, mMicMute %d", device, mMode, mMicMute);
	}
    return do_route_audio_rpc(device,
                  mMode != AudioSystem::MODE_IN_CALL, 
                    (mMode == AudioSystem::MODE_IN_CALL) ? mMicMute : true);
}

status_t AudioHardware::doRouting()
{
    Mutex::Autolock lock(mLock);
    uint32_t outputDevices = mOutput->devices();
    status_t ret = NO_ERROR;
    int audProcess = (ADRC_DISABLE | EQ_DISABLE | RX_IIR_DISABLE);
    AudioStreamInMSM72xx *input = getActiveInput_l();
    uint32_t inputDevice = (input == NULL) ? 0 : input->devices();
    int sndDevice = -1;
    static int counter = 0;

    if (inputDevice != 0) {
        LOGI("do input routing device %x\n", inputDevice);
        if (inputDevice & AudioSystem::DEVICE_IN_BLUETOOTH_SCO_HEADSET) {
            LOGI("Routing audio to Bluetooth PCM\n");
            sndDevice = SND_DEVICE_BT;
        } else if (inputDevice & AudioSystem::DEVICE_IN_WIRED_HEADSET) {
            if ((outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) && (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER)) {       
				LOGI("Routing audio to Wired Headset and Speaker\n");
                sndDevice = SND_DEVICE_SPEAKER;//SND_DEVICE_HEADSET_AND_SPEAKER;
                audProcess = (ADRC_ENABLE | EQ_ENABLE | RX_IIR_ENABLE);
            } else {
                LOGI("Routing audio to Wired Headset\n");
                sndDevice = SND_DEVICE_HEADSET;
            }
        } else {
            if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
				LOGI("Routing audio to Speakerphone\n");
                sndDevice = SND_DEVICE_SPEAKER;
                audProcess = (ADRC_ENABLE | EQ_ENABLE | RX_IIR_ENABLE);
            } else {
                LOGI("Routing audio to Handset\n");
                sndDevice = SND_DEVICE_HANDSET;
            }
        }
    }
    // if inputDevice == 0, restore output routing

    if (sndDevice == -1) {
        if (outputDevices & (outputDevices - 1)) {
            if ((outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) == 0) {
				LOGW("Hardware does not support requested route combination (%#X),"
                     " picking closest possible route...", outputDevices);
            }
        }

        if (outputDevices & (AudioSystem::DEVICE_OUT_BLUETOOTH_SCO | AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_HEADSET)) {
            LOGI("Routing audio to Bluetooth PCM 2\n");
            sndDevice = SND_DEVICE_BT;
            audProcess = (ADRC_DISABLE | EQ_DISABLE | RX_IIR_DISABLE);
        } else if (outputDevices & AudioSystem::DEVICE_OUT_BLUETOOTH_SCO_CARKIT) {
            LOGI("Routing audio to Bluetooth PCM 3\n");
            sndDevice = SND_DEVICE_CARKIT;
            audProcess = (ADRC_DISABLE | EQ_DISABLE | RX_IIR_DISABLE);
        } else if ((outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) && (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER)) {
            LOGI("Routing audio to Wired Headset and Speaker 2\n");
            sndDevice = SND_DEVICE_SPEAKER_MIDI;//SND_DEVICE_HEADSET_AND_SPEAKER;
            audProcess = (ADRC_ENABLE | EQ_ENABLE | RX_IIR_ENABLE);
        } else if (outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADPHONE) {
			if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
				LOGI("Routing audio to No microphone Wired Headset and Speaker 2 (%d,%x)\n", mMode, outputDevices);
                sndDevice = SND_DEVICE_SPEAKER_MIDI;//SND_DEVICE_HEADSET_AND_SPEAKER;
                audProcess = (ADRC_ENABLE | EQ_ENABLE | RX_IIR_ENABLE);
            } else {
                LOGI("Routing audio to No microphone Wired Headset 2 (%d,%x)\n", mMode, outputDevices);
                sndDevice = SND_DEVICE_NO_MIC_HEADSET;
            }
        } else if (outputDevices & AudioSystem::DEVICE_OUT_WIRED_HEADSET) {
			if(mMode != AudioSystem::MODE_IN_CALL) {
				LOGI("Routing audio to Wired Headset 2 normal\n");
				sndDevice = SND_DEVICE_HEADSET;
			} else {
				LOGI("Routing audio to Wired Headset 2 in-call\n");
				sndDevice = SND_DEVICE_HEADSET_INCALL;
			}
        } else if (outputDevices & AudioSystem::DEVICE_OUT_SPEAKER) {
			if(mMode != AudioSystem::MODE_IN_CALL) {
				LOGI("Routing audio to Speakerphone 2 normal\n");
				sndDevice = SND_DEVICE_SPEAKER_MIDI;
			} else {
				LOGI("Routing audio to Speakerphone 2 in-call\n");
				sndDevice = SND_DEVICE_SPEAKER;
			}
                audProcess = (ADRC_ENABLE | EQ_ENABLE | RX_IIR_ENABLE);
        } else {
            LOGI("Routing audio to Handset 2\n");
            sndDevice = SND_DEVICE_HANDSET;
        }
    }
    
    if (LOG_NDEBUG) {
		LOGI("sndDevice: %d mCurSndDevice: %d \n",sndDevice,mCurSndDevice); //ogix
	}

    if (sndDevice != -1 && sndDevice != mCurSndDevice) {
		
		if (LOG_NDEBUG) {
			LOGI("Switching device");
		}
		
		MAX9877_ioctl(MAX9877_AMP_SUSPEND);
        ret = doAudioRouteOrMute(sndDevice);

		if(sndDevice==SND_DEVICE_HEADSET){ //3
			MAX9877_ioctl(MAX9877_SPEAKER_OFF);
			MAX9877_ioctl(MAX9877_HEADSET_OFF);
			MAX9877_ioctl(MAX9877_HEADSET_ON);		
		}
		else if(sndDevice==SND_DEVICE_HEADSET_INCALL){ //2
			MAX9877_ioctl(MAX9877_HEADSET_OFF);
			MAX9877_ioctl(MAX9877_HEADSET_ON);	
		}
		else if(sndDevice==SND_DEVICE_SPEAKER_MIDI){ //26
			MAX9877_ioctl(MAX9877_SPEAKER_ON);
		}
		else if(sndDevice==SND_DEVICE_SPEAKER){ //6
			MAX9877_ioctl(MAX9877_HEADSET_OFF);
		}
		else if(sndDevice==SND_DEVICE_HANDSET){ //0
			MAX9877_ioctl(MAX9877_RCV_ON);
		}
		else if(sndDevice==SND_DEVICE_BT){ //12
			MAX9877_ioctl(MAX9877_RCV_ON);
		}

		do_audpp_enable_rpc(audProcess);
		MAX9877_ioctl(MAX9877_AMP_RESUME);
		mCurSndDevice = sndDevice;

//ogix
/* 
        if ((*msm72xx_enable_audpp) == 0 ) {
            LOGE("Could not open msm72xx_enable_audpp()");
        } else {
            msm72xx_enable_audpp(audProcess);
        }
*/
    }
    return ret;
}

status_t AudioHardware::checkMicMute()
{
    Mutex::Autolock lock(mLock);
    if (mMode != AudioSystem::MODE_IN_CALL) {
        setMicMute_nosync(true);
    }

    return NO_ERROR;
}

status_t AudioHardware::dumpInternals(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioHardware::dumpInternals\n");
    snprintf(buffer, SIZE, "\tmInit: %s\n", mInit? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmMicMute: %s\n", mMicMute? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothNrec: %s\n", mBluetoothNrec? "true": "false");
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmBluetoothId: %d\n", mBluetoothId);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::dump(int fd, const Vector<String16>& args)
{
    dumpInternals(fd, args);
    for (size_t index = 0; index < mInputs.size(); index++) {
        mInputs[index]->dump(fd, args);
    }

    if (mOutput) {
        mOutput->dump(fd, args);
    }
    return NO_ERROR;
}

uint32_t AudioHardware::getInputSampleRate(uint32_t sampleRate)
{
    uint32_t i;
    uint32_t prevDelta;
    uint32_t delta;

    for (i = 0, prevDelta = 0xFFFFFFFF; i < sizeof(inputSamplingRates)/sizeof(uint32_t); i++, prevDelta = delta) {
        delta = abs(sampleRate - inputSamplingRates[i]);
        if (delta > prevDelta) break;
    }
    // i is always > 0 here
    return inputSamplingRates[i-1];
}

// getActiveInput_l() must be called with mLock held
AudioHardware::AudioStreamInMSM72xx *AudioHardware::getActiveInput_l()
{
    for (size_t i = 0; i < mInputs.size(); i++) {
        // return first input found not being in standby mode
        // as only one input can be in this state
        if (mInputs[i]->state() > AudioStreamInMSM72xx::AUDIO_INPUT_CLOSED) {
            return mInputs[i];
        }
    }

    return NULL;
}
// ----------------------------------------------------------------------------

AudioHardware::AudioStreamOutMSM72xx::AudioStreamOutMSM72xx() :
    mHardware(0), mFd(-1), mStartCount(0), mRetryCount(0), mStandby(true), mDevices(0)
{
}

status_t AudioHardware::AudioStreamOutMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate)
{
    int lFormat = pFormat ? *pFormat : 0;
    uint32_t lChannels = pChannels ? *pChannels : 0;
    uint32_t lRate = pRate ? *pRate : 0;

    mHardware = hw;

    // fix up defaults
    if (lFormat == 0) lFormat = format();
    if (lChannels == 0) lChannels = channels();
    if (lRate == 0) lRate = sampleRate();

    // check values
    if ((lFormat != format()) ||
        (lChannels != channels()) ||
        (lRate != sampleRate())) {
        if (pFormat) *pFormat = format();
        if (pChannels) *pChannels = channels();
        if (pRate) *pRate = sampleRate();
        return BAD_VALUE;
    }

    if (pFormat) *pFormat = lFormat;
    if (pChannels) *pChannels = lChannels;
    if (pRate) *pRate = lRate;

    mDevices = devices;

    return NO_ERROR;
}

AudioHardware::AudioStreamOutMSM72xx::~AudioStreamOutMSM72xx()
{
    if (mFd >= 0) close(mFd);
}

ssize_t AudioHardware::AudioStreamOutMSM72xx::write(const void* buffer, size_t bytes)
{
    // LOGD("AudioStreamOutMSM72xx::write(%p, %u)", buffer, bytes);
    status_t status = NO_INIT;
    size_t count = bytes;
    const uint8_t* p = static_cast<const uint8_t*>(buffer);

    if (mStandby) {

        // open driver
        LOGV("open driver");
        status = ::open("/dev/msm_pcm_out", O_RDWR);
        if (status < 0) {
            LOGE("Cannot open /dev/msm_pcm_out errno: %d", errno);
            goto Error;
        }
        mFd = status;

        // configuration
        LOGV("get config");
        struct msm_audio_config config;
        status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
        if (status < 0) {
            LOGE("Cannot read config");
            goto Error;
        }

        LOGV("set config");
        config.channel_count = AudioSystem::popCount(channels());
        config.sample_rate = sampleRate();
        config.buffer_size = bufferSize();
        config.buffer_count = AUDIO_HW_NUM_OUT_BUF;
        config.codec_type = CODEC_TYPE_PCM;
        status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
        if (status < 0) {
            LOGE("Cannot set config");
            goto Error;
        }

        LOGV("buffer_size: %u", config.buffer_size);
        LOGV("buffer_count: %u", config.buffer_count);
        LOGV("channel_count: %u", config.channel_count);
        LOGV("sample_rate: %u", config.sample_rate);

        // fill 2 buffers before AUDIO_START
        mStartCount = AUDIO_HW_NUM_OUT_BUF;
        mStandby = false;
    }

    while (count) {
        ssize_t written = ::write(mFd, p, count);
        if (written >= 0) {
            count -= written;
            p += written;
        } else {
            if (errno != EAGAIN) return written;
            mRetryCount++;
            LOGW("EAGAIN - retry");
        }
    }

    // start audio after we fill 2 buffers
    if (mStartCount) {
        if (--mStartCount == 0) {
            ioctl(mFd, AUDIO_START, 0);
        }
    }
    return bytes;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    // Simulate audio output timing in case of error
    usleep(bytes * 1000000 / frameSize() / sampleRate());

    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::standby()
{
    status_t status = NO_ERROR;
    if (!mStandby && mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    mStandby = true;
    return status;
}

status_t AudioHardware::AudioStreamOutMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamOutMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStartCount: %d\n", mStartCount);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmStandby: %s\n", mStandby? "true": "false");
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

bool AudioHardware::AudioStreamOutMSM72xx::checkStandby()
{
    return mStandby;
}


status_t AudioHardware::AudioStreamOutMSM72xx::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    LOGV("AudioStreamOutMSM72xx::setParameters() %s", keyValuePairs.string());

    if (param.getInt(key, device) == NO_ERROR) {
        mDevices = device;
        LOGV("set output routing %x", mDevices);
        status = mHardware->doRouting();
        param.remove(key);
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamOutMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        LOGV("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    LOGV("AudioStreamOutMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

status_t AudioHardware::AudioStreamOutMSM72xx::getRenderPosition(uint32_t *dspFrames)
{
    //TODO: enable when supported by driver
    return INVALID_OPERATION;
}

// ----------------------------------------------------------------------------

AudioHardware::AudioStreamInMSM72xx::AudioStreamInMSM72xx() :
    mHardware(0), mFd(-1), mState(AUDIO_INPUT_CLOSED), mRetryCount(0),
    mFormat(AUDIO_HW_IN_FORMAT), mChannels(AUDIO_HW_IN_CHANNELS),
    mSampleRate(AUDIO_HW_IN_SAMPLERATE), mBufferSize(AUDIO_HW_IN_BUFFERSIZE),
    mAcoustics((AudioSystem::audio_in_acoustics)0), mDevices(0)
{
}

status_t AudioHardware::AudioStreamInMSM72xx::set(
        AudioHardware* hw, uint32_t devices, int *pFormat, uint32_t *pChannels, uint32_t *pRate,
        AudioSystem::audio_in_acoustics acoustic_flags)
{
    if (pFormat == 0 || *pFormat != AUDIO_HW_IN_FORMAT) {
        *pFormat = AUDIO_HW_IN_FORMAT;
        return BAD_VALUE;
    }
    if (pRate == 0) {
        return BAD_VALUE;
    }
    uint32_t rate = hw->getInputSampleRate(*pRate);
    if (rate != *pRate) {
        *pRate = rate;
        return BAD_VALUE;
    }

    if (pChannels == 0 || (*pChannels != AudioSystem::CHANNEL_IN_MONO &&
        *pChannels != AudioSystem::CHANNEL_IN_STEREO)) {
        *pChannels = AUDIO_HW_IN_CHANNELS;
        return BAD_VALUE;
    }

    mHardware = hw;

    LOGV("AudioStreamInMSM72xx::set(%d, %d, %u)", *pFormat, *pChannels, *pRate);
    if (mFd >= 0) {
        LOGE("Audio record already open");
        return -EPERM;
    }

    // open audio input device
    status_t status = ::open("/dev/msm_pcm_in", O_RDWR);
    if (status < 0) {
        LOGE("Cannot open /dev/msm_pcm_in errno: %d", errno);
        goto Error;
    }
    mFd = status;

    // configuration
    LOGV("get config");
    struct msm_audio_config config;
    status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot read config");
        goto Error;
    }

    LOGV("set config");
    config.channel_count = AudioSystem::popCount(*pChannels);
    config.sample_rate = *pRate;
    config.buffer_size = bufferSize();
    config.buffer_count = 2;
    config.codec_type = CODEC_TYPE_PCM;
    status = ioctl(mFd, AUDIO_SET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot set config");
        if (ioctl(mFd, AUDIO_GET_CONFIG, &config) == 0) {
            if (config.channel_count == 1) {
                *pChannels = AudioSystem::CHANNEL_IN_MONO;
            } else {
                *pChannels = AudioSystem::CHANNEL_IN_STEREO;
            }
            *pRate = config.sample_rate;
        }
        goto Error;
    }

    LOGV("confirm config");
    status = ioctl(mFd, AUDIO_GET_CONFIG, &config);
    if (status < 0) {
        LOGE("Cannot read config");
        goto Error;
    }
    LOGV("buffer_size: %u", config.buffer_size);
    LOGV("buffer_count: %u", config.buffer_count);
    LOGV("channel_count: %u", config.channel_count);
    LOGV("sample_rate: %u", config.sample_rate);

    mDevices = devices;
    mFormat = AUDIO_HW_IN_FORMAT;
    mChannels = *pChannels;
    mSampleRate = config.sample_rate;
    mBufferSize = config.buffer_size;

    //mHardware->setMicMute_nosync(false);
    mState = AUDIO_INPUT_OPENED;

    if (!acoustic)
        return NO_ERROR;

    audpre_index = calculate_audpre_table_index(mSampleRate);
    tx_iir_index = (audpre_index * 2) + (hw->checkOutputStandby() ? 0 : 1);
    LOGD("audpre_index = %d, tx_iir_index = %d\n", audpre_index, tx_iir_index);

    /**
     * If audio-preprocessing failed, we should not block record.
     */
    int (*msm72xx_set_audpre_params)(int, int);
    msm72xx_set_audpre_params = (int (*)(int, int))::dlsym(acoustic, "msm72xx_set_audpre_params");
    status = msm72xx_set_audpre_params(audpre_index, tx_iir_index);
    if (status < 0)
        LOGE("Cannot set audpre parameters");

    int (*msm72xx_enable_audpre)(int, int, int);
    msm72xx_enable_audpre = (int (*)(int, int, int))::dlsym(acoustic, "msm72xx_enable_audpre");
    mAcoustics = acoustic_flags;
    status = msm72xx_enable_audpre((int)acoustic_flags, audpre_index, tx_iir_index);
    if (status < 0)
        LOGE("Cannot enable audpre");

    return NO_ERROR;

Error:
    if (mFd >= 0) {
        ::close(mFd);
        mFd = -1;
    }
    return status;
}

AudioHardware::AudioStreamInMSM72xx::~AudioStreamInMSM72xx()
{
    LOGV("AudioStreamInMSM72xx destructor");
    standby();
}

ssize_t AudioHardware::AudioStreamInMSM72xx::read( void* buffer, ssize_t bytes)
{
    LOGV("AudioStreamInMSM72xx::read(%p, %ld)", buffer, bytes);
    if (!mHardware) return -1;

    size_t count = bytes;
    uint8_t* p = static_cast<uint8_t*>(buffer);

    if (mState < AUDIO_INPUT_OPENED) {
        Mutex::Autolock lock(mHardware->mLock);
        if (set(mHardware, mDevices, &mFormat, &mChannels, &mSampleRate, mAcoustics) != NO_ERROR) {
            return -1;
        }
    }

    if (mState < AUDIO_INPUT_STARTED) {
        mState = AUDIO_INPUT_STARTED;
        // force routing to input device
        mHardware->clearCurDevice();
        mHardware->doRouting();
        if (ioctl(mFd, AUDIO_START, 0)) {
            LOGE("Error starting record");
            standby();
            return -1;
        }
    }

    while (count) {
        ssize_t bytesRead = ::read(mFd, buffer, count);
        if (bytesRead >= 0) {
            count -= bytesRead;
            p += bytesRead;
        } else {
            if (errno != EAGAIN) return bytesRead;
            mRetryCount++;
            LOGW("EAGAIN - retrying");
        }
    }
    return bytes;
}

status_t AudioHardware::AudioStreamInMSM72xx::standby()
{
    if (mState > AUDIO_INPUT_CLOSED) {
        if (mFd >= 0) {
            ::close(mFd);
            mFd = -1;
        }
        mState = AUDIO_INPUT_CLOSED;
    }
    if (!mHardware) return -1;
    // restore output routing if necessary
    mHardware->clearCurDevice();
    mHardware->doRouting();
    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::dump(int fd, const Vector<String16>& args)
{
    const size_t SIZE = 256;
    char buffer[SIZE];
    String8 result;
    result.append("AudioStreamInMSM72xx::dump\n");
    snprintf(buffer, SIZE, "\tsample rate: %d\n", sampleRate());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tbuffer size: %d\n", bufferSize());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tchannels: %d\n", channels());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tformat: %d\n", format());
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmHardware: %p\n", mHardware);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmFd count: %d\n", mFd);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmState: %d\n", mState);
    result.append(buffer);
    snprintf(buffer, SIZE, "\tmRetryCount: %d\n", mRetryCount);
    result.append(buffer);
    ::write(fd, result.string(), result.size());
    return NO_ERROR;
}

status_t AudioHardware::AudioStreamInMSM72xx::setParameters(const String8& keyValuePairs)
{
    AudioParameter param = AudioParameter(keyValuePairs);
    String8 key = String8(AudioParameter::keyRouting);
    status_t status = NO_ERROR;
    int device;
    LOGV("AudioStreamInMSM72xx::setParameters() %s", keyValuePairs.string());

    if (param.getInt(key, device) == NO_ERROR) {
        LOGV("set input routing %x", device);
        if (device & (device - 1)) {
            status = BAD_VALUE;
        } else {
            mDevices = device;
            status = mHardware->doRouting();
        }
        param.remove(key);
    }

    if (param.size()) {
        status = BAD_VALUE;
    }
    return status;
}

String8 AudioHardware::AudioStreamInMSM72xx::getParameters(const String8& keys)
{
    AudioParameter param = AudioParameter(keys);
    String8 value;
    String8 key = String8(AudioParameter::keyRouting);

    if (param.get(key, value) == NO_ERROR) {
        LOGV("get routing %x", mDevices);
        param.addInt(key, (int)mDevices);
    }

    LOGV("AudioStreamInMSM72xx::getParameters() %s", param.toString().string());
    return param.toString();
}

// ----------------------------------------------------------------------------

extern "C" AudioHardwareInterface* createAudioHardware(void) {
    return new AudioHardware();
}

}; // namespace android

