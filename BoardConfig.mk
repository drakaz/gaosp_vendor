# config.mk
# 
# Product-specific compile-time definitions.
#
TARGET_BOARD_PLATFORM := msm7k
TARGET_BOARD_PLATFORM_GPU := qcom
TARGET_CPU_ABI := armeabi
TARGET_ARCH_VARIANT := armv5te

# JIT
# WITH_JIT := true

BUILD_OLD_SU := false

BOARD_HAVE_BLUETOOTH    := true
BOARD_HAVE_BLUETOOTH_BCM    := true
BT_USE_BTL_IF := true
BT_ALT_STACK := true
BRCM_BTL_INCLUDE_A2DP := true
BRCM_BT_USE_BTL_IF := true

USE_QEMU_GPS_HARDWARE   := false
BOARD_HAVE_GPS_HARDWARE := true
BOARD_GPS_LIBRARIES := libgps librpc

BOARD_USES_ALSA_AUDIO   := true
BUILD_WITH_ALSA_UTILS := true
BOARD_USES_GENERIC_AUDIO := false
HAVE_HTC_AUDIO_DRIVER := false

BOARD_USES_OLD_CAMERA_HACK := true
USE_CAMERA_STUB := false

TARGET_HARDWARE_3D := false
BOARD_GL_TEX_POW2_DIMENSION_REQUIRED := true
BOARD_NO_GL2 := true
BOARD_USES_QCOM_LIBS := true

BOARD_KERNEL_CMDLINE := no_console_suspend=1
USE_PV_WINDOWS_MEDIA := false

WPA_BUILD_SUPPLICANT := true
BOARD_WPA_SUPPLICANT_DRIVER := true

TARGET_NO_BOOTLOADER := true
TARGET_NO_KERNEL := true
TARGET_NO_RADIOIMAGE := true
TARGET_SIMULATOR := false

TARGET_PROVIDES_INIT_RC := true

PRIVATE_BUILD_DESC := passion-user 2.1-update1 ERE27 24178 release-keys
BUILD_FINGERPRINT := google/passion/passion/mahimahi:2.1-update1/ERE27/24178:user/release-keys



















