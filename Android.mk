include $(call all-named-subdir-makefiles, akmd_free libaudio libcamera libsensors)

#Android makefile to build kernel as a part of Android Build
#Its here and not under gaosp-kernel/ for compatiblity with the froyo branch
#The config for the kernel is arch/arm/configs/gaosp_msm_defconfig
LOCAL_PATH := $(call my-dir)/../../cm-kernel
KERNEL_OUT := $(TARGET_OUT_INTERMEDIATES)/kernel
KERNEL_CONFIG := $(KERNEL_OUT)/.config
TARGET_PREBUILT_KERNEL := $(KERNEL_OUT)/arch/arm/boot/zImage

$(INSTALLED_KERNEL_TARGET): $(TARGET_PREBUILT_KERNEL) | $(ACP)
	$(transform-prebuilt-to-target)

$(KERNEL_OUT):
	mkdir -p $(KERNEL_OUT)

$(KERNEL_CONFIG): $(LOCAL_PATH)/arch/arm/configs/gaosp_msm_defconfig | $(ACP)
	$(transform-prebuilt-to-target)

$(TARGET_PREBUILT_KERNEL): $(KERNEL_OUT) $(KERNEL_CONFIG)
	$(MAKE) -C cm-kernel O=../$(KERNEL_OUT) ARCH=arm CROSS_COMPILE=$(shell pwd)/prebuilt/linux-x86/toolchain/arm-eabi-4.4.0/bin/arm-eabi- INSTALL_MOD_STRIP=1
