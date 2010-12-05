# config.mk
# 
# Product-specific compile-time definitions.
#

USE_CAMERA_STUB := false
TARGET_EMULATOR := false

TARGET_BOARD_PLATFORM := msm7k
TARGET_BOARD_PLATFORM_GPU := qcom
TARGET_CPU_ABI := armeabi
TARGET_ARCH_VARIANT := armv6j
TARGET_KERNEL_CONFIG := gaosp_msm_defconfig
TARGET_KERNEL_DIR := gaosp-kernel

# JIT
WITH_JIT := true

BUILD_OLD_SU := false

BOARD_HAVE_BLUETOOTH    := true
BOARD_HAVE_BLUETOOTH_BCM    := true
BOARD_VENDOR_USE_AKMD := akm8973
BT_USE_BTL_IF := true
BT_ALT_STACK := true
BRCM_BTL_INCLUDE_A2DP := true
BRCM_BT_USE_BTL_IF := true
WITH_A2DP := true

# OpenGL drivers config file path
BOARD_EGL_CFG := device/Samsung/I7500/open/lib/egl.cfg

USE_QEMU_GPS_HARDWARE   := false
BOARD_HAVE_GPS_HARDWARE := true
BOARD_GPS_LIBRARIES :=  libloc_api

#Wifi
WIFI_DRIVER_FW_AP_PATH = "/system/etc/rtecdc.bin"
BOARD_WPA_SUPPLICANT_DRIVER := WEXT
WPA_SUPPLICANT_VERSION := VER_0_6_X
WIFI_DRIVER_MODULE_PATH := "/system/lib/modules/bcm4329.ko"
#WIFI_DRIVER_MODULE_ARG := "iface_name=wlan0"
WIFI_DRIVER_MODULE_NAME := "bcm4329"

#BOARD_USES_ALSA_AUDIO   := true
#BUILD_WITH_ALSA_UTILS := true
BOARD_USES_GENERIC_AUDIO := false
HAVE_HTC_AUDIO_DRIVER := false

BOARD_USE_I7500_USB_FUNCTION_SWITCH := true

# Use libcamera2
BOARD_USES_OLD_CAMERA_HACK := true

# No authoring clock for OpenCore
BOARD_NO_PV_AUTHORING_CLOCK := true

TARGET_HARDWARE_3D := false
BOARD_GL_TEX_POW2_DIMENSION_REQUIRED := true
BOARD_NO_GL2 := true
BOARD_USES_QCOM_LIBS := true

BOARD_KERNEL_CMDLINE := no_console_suspend=1
USE_PV_WINDOWS_MEDIA := false

TARGET_NO_RECOVERY := true
TARGET_NO_BOOTLOADER := true
TARGET_NO_RADIOIMAGE := true
TARGET_SIMULATOR := false

TARGET_PROVIDES_INIT_RC := true














