LOCAL_PATH:= $(call my-dir)
include $(CLEAR_VARS)
LOCAL_PRELINK_MODULE := false
LOCAL_MODULE_PATH := $(TARGET_OUT_SHARED_LIBRARIES)/hw
LOCAL_SHARED_LIBRARIES := liblog libcutils
#LOCAL_STATIC_LIBRARIES := libcutils libc libAK8973
#ifeq ($(TARGET_DEVICE),surf)
#LOCAL_SRC_FILES += sensors_atos.c
#LOCAL_SRC_FILES += sensors_mma.c
LOCAL_SRC_FILES += sensors_ak8973.c
#LOCAL_SRC_FILES += sensors_proximity.c 
#endif
LOCAL_MODULE := sensors.samsung
LOCAL_MODULE_TAGS := optional
include $(BUILD_SHARED_LIBRARY)

