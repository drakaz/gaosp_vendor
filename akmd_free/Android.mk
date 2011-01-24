LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_ARM_MODE := arm
LOCAL_SRC_FILES := Akmd.cpp Calibrator.cpp main.cpp \
    math/Matrix.cpp math/Vector.cpp math/Filter.cpp\
    device/AK8973B.cpp \
    device/BMA020.cpp \
    device/OrientationAdapter.cpp device/TemperatureReaderAdapter.cpp
LOCAL_SHARED_LIBRARIES := liblog
LOCAL_MODULE := akmd2
LOCAL_MODULE_TAGS := optional

include $(BUILD_EXECUTABLE)
