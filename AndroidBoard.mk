LOCAL_PATH := $(call my-dir)

# Kernel
ifeq ($(TARGET_PREBUILT_KERNEL),)
TARGET_PREBUILT_KERNEL := $(LOCAL_PATH)/open/boot/kernel
endif
TARGET_PREBUILT_KERNEL := $(LOCAL_PATH)/open/boot/kernel

file := $(INSTALLED_KERNEL_TARGET)
ALL_PREBUILT += $(file)
$(file): $(TARGET_PREBUILT_KERNEL) | $(ACP)
$(transform-prebuilt-to-target)

# Keylayouts
file := $(TARGET_OUT_KEYLAYOUT)/I7500_headset.kl
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/keylayout/I7500_headset.kl | $(ACP)
	$(transform-prebuilt-to-target)


file := $(TARGET_OUT_KEYLAYOUT)/I7500_keypad.kl
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/keylayout/I7500_keypad.kl | $(ACP)
	$(transform-prebuilt-to-target)


include $(CLEAR_VARS)
LOCAL_SRC_FILES := /open/keylayout/azerty.kcm
include $(BUILD_KEY_CHAR_MAP)

# init.rc
file := $(TARGET_ROOT_OUT)/init.rc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/boot/init.rc | $(ACP)
	$(transform-prebuilt-to-target)

# open etc
file := $(TARGET_OUT)/etc/gps.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/gps.conf | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/nvram.txt
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/nvram.txt | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/vold.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/vold.conf | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/wifi/wpa_supplicant.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/wpa_supplicant.conf | $(ACP)
	$(transform-prebuilt-to-target)

# open modules
file := $(TARGET_OUT)/lib/modules/bcm4325.ko
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/modules/bcm4325.ko | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/modules/dpram.ko
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/modules/dpram.ko | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/modules/multipdp.ko
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/modules/multipdp.ko | $(ACP)
	$(transform-prebuilt-to-target)

# Proprietary etc
file := $(TARGET_OUT)/etc/rtecdc.bin
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/etc/rtecdc.bin | $(ACP)
	$(transform-prebuilt-to-target)


# Proprietary bin
file := $(TARGET_OUT)/bin/akmd2
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/bin/akmd2 | $(ACP)
	$(transform-prebuilt-to-target)

# Proprietary lib
file := $(TARGET_OUT)/lib/libsec-ril.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libsec-ril.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/copybit.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/hw/copybit.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/lights.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/hw/lights.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/sensors.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/hw/sensors.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)




