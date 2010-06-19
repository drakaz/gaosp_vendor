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

# boot files
file := $(TARGET_ROOT_OUT)/init.rc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/boot/init.rc | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_ROOT_OUT)/logo.rle
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/boot/logo.rle | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_ROOT_OUT)/init
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/boot/init | $(ACP)
	$(transform-prebuilt-to-target)

# bootanimation
file := $(TARGET_OUT)/media/bootanimation.zip
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/boot/bootanimation.zip | $(ACP)
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

file := $(TARGET_OUT)/etc/gaosp.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/gaosp.conf | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/wifi/wpa_supplicant.conf
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/wpa_supplicant.conf | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/fstab
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/fstab | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/bash.bashrc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/bash.bashrc | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/gshadow
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/gshadow | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/passwd
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/passwd | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/profile
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/profile | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/shadow
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/shadow | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/group
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/group | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/shells
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/shells | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/zshrc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/zshrc | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/terminfo/l/linux
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/terminfo/l/linux | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/etc/terminfo/u/unknown
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/etc/terminfo/u/unknown | $(ACP)
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

file := $(TARGET_OUT)/lib/modules/ramzswap.ko
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/modules/ramzswap.ko | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/modules/tun.ko
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/modules/tun.ko | $(ACP)
	$(transform-prebuilt-to-target)

# open bin
file := $(TARGET_OUT)/xbin/dropbear
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/dropbear | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/dropbearkey
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/dropbearkey | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/usb-tether
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/usb-tether | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/shutdown
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/shutdown | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/bin/rc
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/rc | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/remountro
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/remountro | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/remountrw
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/remountrw | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/rmmod
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/rmmod | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/rzcontrol
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/rzcontrol | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/scp
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/scp | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/sqlite3
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/sqlite3 | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/ssh
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/ssh | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/vim
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/vim | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/xbin/zsh
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/zsh | $(ACP)
	$(transform-prebuilt-to-target)

###########
# busybox #
###########

file := $(TARGET_OUT)/xbin/busybox
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/bin/busybox | $(ACP)
	$(transform-prebuilt-to-target)

links := $(shell cat $(LOCAL_PATH)/busybox.links)
exclude := nc
SYMLINKS := $(addprefix $(TARGET_OUT)/xbin/,$(filter-out $(exclude),$(notdir $(links))))
$(SYMLINKS): BUSYBOX_BINARY := busybox
$(SYMLINKS): $(TARGET_OUT)/xbin/busybox
	@echo "Symlink: $@ -> $(BUSYBOX_BINARY)"
	@mkdir -p $(dir $@)
	@rm -rf $@
	$(hide) ln -sf $(BUSYBOX_BINARY) $@
ALL_DEFAULT_INSTALLED_MODULES += $(SYMLINKS)
 
# We need this so that the installed files could be picked up based on the
# local module name
ALL_MODULES.$(LOCAL_MODULE).INSTALLED := \
    $(ALL_MODULES.$(LOCAL_MODULE).INSTALLED) $(SYMLINKS)

###############
# end busybox #
###############

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

file := $(TARGET_OUT)/bin/BCM4325D1_004.002.004.0153.0156.hcd
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/bin/BCM4325D1_004.002.004.0153.0156.hcd | $(ACP)
	$(transform-prebuilt-to-target)



# Proprietary lib
file := $(TARGET_OUT)/lib/libsec-ril.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libsec-ril.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/libloc_api.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/libloc_api.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/lights.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/lib/hw/lights.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

file := $(TARGET_OUT)/lib/hw/sensors.samsung.so
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/open/lib/hw/sensors.samsung.so | $(ACP)
	$(transform-prebuilt-to-target)

# Keylayouts
file := $(TARGET_OUT)/usr/keychars/I7500_keypad.kcm.bin
ALL_PREBUILT += $(file)
$(file) : $(LOCAL_PATH)/proprietary/keychars/I7500_keypad.kcm.bin | $(ACP)
	$(transform-prebuilt-to-target)






