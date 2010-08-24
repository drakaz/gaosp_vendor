LOCAL_PATH := $(call my-dir)

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

# boot files
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

file := $(TARGET_OUT)/etc/vold.fstab
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/vold.fstab | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/wifi/wpa_supplicant.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/wpa_supplicant.conf | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/dhcpcd/dhcpcd.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/dhcpcd.conf | $(ACP)
	$(transform-prebuilt-to-target)

# Proprietary etc
file := $(TARGET_OUT)/etc/rtecdc.bin
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/etc/rtecdc.bin | $(ACP)
	$(transform-prebuilt-to-target)

# Proprietary lib
file := $(TARGET_OUT)/lib/libsec-ril.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libsec-ril.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/lights.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/hw/lights.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/sensors.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/lib/hw/sensors.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

ifdef WITH_WINDOWS_MEDIA

file := $(TARGET_OUT)/lib/libomx_wmadec_sharedlibrary.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libomx_wmadec_sharedlibrary.so | $(ACP)
        $(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/libomx_wmvdec_sharedlibrary.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libomx_wmvdec_sharedlibrary.so | $(ACP)
        $(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/libpvasfcommon.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libpvasfcommon.so | $(ACP)
        $(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/libpvasflocalpbreg.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libpvasflocalpbreg.so: | $(ACP)
        $(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/libpvasflocalpb.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libpvasflocalpb.so | $(ACP)
        $(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/pvasflocal.cfg
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/pvasflocal.cfg | $(ACP)
        $(transform-prebuilt-to-target)

endif

# Keylayouts
file := $(TARGET_OUT)/usr/keychars/I7500_keypad.kcm.bin
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/keychars/I7500_keypad.kcm.bin | $(ACP)
	$(transform-prebuilt-to-target)




