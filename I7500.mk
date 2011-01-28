WITH_GOOGLE := true

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=GRH78 BUILD_DISPLAY_ID=GRH78 BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2.1/FRG83D/75603:user/release-keys
PRIVATE_BUILD_DESC="passion-user 2.2.1 FRG83D 75603 release-keys"
DEVICE_PACKAGE_OVERLAYS := device/samsung/I7500/overlay

PACKAGES.Email.OVERRIDES := Provision LatinIME QuickSearchBox

# Prebuilt kernel
PRODUCT_SPECIFIC_DEFINES += TARGET_PREBUILT_KERNEL=device/samsung/I7500/open/boot/kernel
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_DIR=gaosp-kernel
PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_CONFIG=gaosp_msm_defconfig
TARGET_PREBUILT_KERNEL := device/samsung/I7500/open/boot/kernel
LOCAL_KERNEL := device/samsung/I7500/open/boot/kernel

# Used by BusyBox
KERNEL_MODULES_DIR:=/system/lib/modules

# Tiny toolbox
TINY_TOOLBOX:=false

# Enable Windows Media if supported by the board
WITH_WINDOWS_MEDIA:=true

NO_DEFAULT_SOUNDS := true
PRODUCT_POLICY := android.policy_phone

# PROPERTIES
PRODUCT_PROPERTY_OVERRIDES := \
    keyguard.no_require_sim=true \
    dalvik.vm.dexopt-data-only=0 \
    ro.ril.hsxpa=2 \
    ro.ril.gprsclass=10 \
    ro.media.dec.jpeg.memcap=10000000 \
    ro.sf.lcd_density=160 \
    ro.telephony.default_network=0 \
    dalvik.vm.execution-mode=int:fast \
    ro.opengles.version=65536 \
    ro.url.legal=http://www.google.com/intl/%s/mobile/android/basic/phone-legal.html \
    ro.url.legal.android_privacy=http://www.google.com/intl/%s/mobile/android/basic/privacy.html \
    ro.com.google.clientidbase=android-google \
    ro.com.android.wifi-watchlist=GoogleGuest \
    ro.setupwizard.enterprise_mode=1 \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.com.android.dataroaming=false \
    net.bt.name=Galaxy 

# Enable Compcache by default
PRODUCT_PROPERTY_OVERRIDES += \
    ro.compcache.default=18

# Disable fs check on boot by default
PRODUCT_PROPERTY_OVERRIDES += \
    sys.checkfs.fat=false

# Chinese specific
ifdef CHINESE
    PRODUCT_PROPERTY_OVERRIDES += \
	ro.com.android.dateformat=yyyy-MM-dd 
else
    PRODUCT_PROPERTY_OVERRIDES += \
    	ro.com.android.dateformat=MM-dd-yyyy 
endif

# Nightly
ifdef NIGHTLY
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=GAOSP-3-$(shell date +%m%d%Y)-NIGHTLY
else
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=GAOSP-3-PUBLIC-BETA-1
endif

# Other Files
PRODUCT_COPY_FILES += device/samsung/I7500/open/etc/apns-conf.xml:system/etc/apns-conf.xml \
	frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
	device/samsung/I7500/open/etc/com.tmobile.software.themes.xml:system/etc/permissions/com.tmobile.software.themes.xml \
	frameworks/base/data/etc/android.software.sip.voip.xml:system/etc/permissions/android.software.sip.voip.xml  \
	device/samsung/I7500/open/etc/media_profiles.xml:system/etc/media_profiles.xml \
	device/samsung/I7500/open/etc/resolv.conf:system/etc/resolv.conf \
	device/samsung/I7500/open/etc/sysctl.conf:system/etc/sysctl.conf \
	device/samsung/I7500/open/etc/terminfo/l/linux:system/etc/terminfo/l/linux \
	device/samsung/I7500/open/etc/terminfo/u/unknown:system/etc/terminfo/u/unknown \
	device/samsung/I7500/open/etc/fstab:system/etc/fstab \
	device/samsung/I7500/open/etc/bash.bashrc:system/etc/bash.bashrc \
	device/samsung/I7500/open/etc/gshadow:system/etc/gshadow \
	device/samsung/I7500/open/etc/passwd:system/etc/passwd \
	device/samsung/I7500/open/etc/profile:system/etc/profile \
	device/samsung/I7500/open/etc/shadow:system/etc/shadow \
	device/samsung/I7500/open/etc/group:system/etc/group \
	device/samsung/I7500/open/etc/shells:system/etc/shells \
	device/samsung/I7500/open/etc/zshrc:system/etc/zshrc \
	device/samsung/I7500/open/etc/adb_shell.conf:system/etc/adb_shell.conf \
	device/samsung/I7500/open/etc/gaosp.conf:system/etc/gaosp.conf \
	device/samsung/I7500/open/bin/rc:system/bin/rc \
	device/samsung/I7500/open/bin/compcache:system/bin/compcache \
	device/samsung/I7500/open/bin/handle_compcache:system/bin/handle_compcache \
	device/samsung/I7500/open/bin/fix_permissions:system/bin/fix_permissions \
	device/samsung/I7500/open/bin/openvpn-up.sh:system/xbin/openvpn-up.sh \
	device/samsung/I7500/open/bin/htop:system/xbin/htop \
	device/samsung/I7500/open/bin/irssi:system/xbin/irssi \
	device/samsung/I7500/open/bin/lsof:system/xbin/lsof \
	device/samsung/I7500/open/bin/powertop:system/xbin/powertop \
	device/samsung/I7500/open/bin/shutdown:system/xbin/shutdown \
	device/samsung/I7500/open/bin/voldupdater:system/xbin/voldupdater \
	device/samsung/I7500/open/bin/remountro:system/xbin/remountro \
	device/samsung/I7500/open/bin/remountrw:system/xbin/remountrw \
	device/samsung/I7500/open/bin/zsh:system/xbin/zsh \
	device/samsung/I7500/open/bin/bash:system/bin/bash \
	device/samsung/I7500/open/bin/sysinit:system/bin/sysinit \
	device/samsung/I7500/open/bin/fbvncserver:system/bin/fbvncserver \
	device/samsung/I7500/open/bin/changepasswd:system/bin/changepasswd \
	device/samsung/I7500/open/bin/dropbear:system/xbin/dropbear \
	device/samsung/I7500/open/bin/dropbear-keygen:system/xbin/dropbear-keygen \
	device/samsung/I7500/open/bin/dropbearkey:system/xbin/dropbearkey \
	device/samsung/I7500/open/bin/busybox:system/xbin/busybox \
	device/samsung/I7500/proprietary/bin/BCM4325D1_004.002.004.0153.0156.hcd:system/bin/BCM4325D1_004.002.004.0153.0156.hcd \
	device/samsung/I7500/open/apps/serviceModeApp.apk:system/app/serviceModeApp.apk \
	device/samsung/I7500/open/apps/HotRebootFree.apk:system/app/HotRebootFree.apk \
	device/samsung/I7500/open/boot/bootanimation.zip:system/media/bootanimation.zip \
	device/samsung/I7500/open/boot/bootanimation_old.zip:system/media/bootanimation_old.zip \
	device/samsung/I7500/open/boot/logo.rle:root/logo.rle \
	device/samsung/I7500/open/modules/ramzswap.ko:system/lib/modules/ramzswap.ko \
	device/samsung/I7500/open/modules/bcm4329.ko:system/lib/modules/bcm4329.ko \
	device/samsung/I7500/open/lib/librun.so:system/lib/librun.so \
	device/samsung/I7500/open/etc/init.d/00banner:system/etc/init.d/00banner \
	device/samsung/I7500/open/etc/init.d/01sysctl:system/etc/init.d/01sysctl \
	device/samsung/I7500/open/etc/init.d/03firstboot:system/etc/init.d/03firstboot \
	device/samsung/I7500/open/etc/init.d/04modules:system/etc/init.d/04modules \
	device/samsung/I7500/open/etc/init.d/20userinit:system/etc/init.d/20userinit \
        device/samsung/I7500/open/etc/rtecdc_adhoc.bin:system/etc/rtecdc.bin \
        device/samsung/I7500/open/boot/ueventd.samsung.rc:root/ueventd.samsung.rc \
	$(LOCAL_KERNEL):kernel \
	device/samsung/I7500/open/modules/multipdp.ko:system/lib/modules/multipdp.ko \
	device/samsung/I7500/open/modules/dpram.ko:system/lib/modules/dpram.ko \
	device/samsung/I7500/open/modules/tun.ko:system/lib/modules/tun.ko

# Sounds
#include frameworks/base/data/sounds/AudioPackage4.mk
# TTS languages
#include external/svox/pico/lang/PicoLangDeDeInSystem.mk
#include external/svox/pico/lang/PicoLangEnGBInSystem.mk
#include external/svox/pico/lang/PicoLangEnUsInSystem.mk
#include external/svox/pico/lang/PicoLangEsEsInSystem.mk
#include external/svox/pico/lang/PicoLangFrFrInSystem.mk
#include external/svox/pico/lang/PicoLangItItInSystem.mk

# Packages
PRODUCT_PACKAGES := \
    azerty.kcm \
    framework-res \
    AlarmClock \
    AlarmProvider \
    Androidian \
    AndroidTerm \
    AccountAndSyncSettings \
    ADWLauncher \
    AndroidTerm \
    ApplicationsProvider \
    Bluetooth \
    Browser \
    Bugreport \
    Calculator \
    Calendar \
    CalendarProvider \
    Camera \
    CertInstaller \
    CMParts \
    CMPartsHelper \
    Contacts \
    ContactsProvider \
    CustomLocale \
    Cyanbread \
    DefaultContainerService \
    DownloadProvider \
    DeskClock \
    DSPManager \
    Email \
    Gallery \
    Gallery3D \
    GaospConf \
    GoogleContactsProvider \
    GoogleSearch \
    HTMLViewer \
    IM \
    ImProvider \
    klogtail \
    ManagementProvider \
    MediaProvider \
    Mms \
    Music \
    PackageInstaller \
    Pacman \
    Phone \
    PicoTts \
    Protips \
    sensors.samsung \
    Settings \
    SettingsProvider \
    SoundRecoder \
    SubscribedFeedsProvider \
    Stk \
    TelephonyProvider \
    Term \
    ThemeManager \
    ThemeChooser \
    com.tmobile.themes \
    TtsService \
    Updater \
    UserDictionaryProvider \
    VoiceDialer \
    VpnServices \
    WebSearchProvider \
    LiveWallpapersPicker \
    FileManager \
    Superuser \
    Torch \
    libjni_flash \
    sensors.msm7k \
    libcamera \
    libmmcamera.so \
    libmm-qcamera-tgt.so \
    libmmjpeg.so \
    libterm \
    libOmxCore \
    libmm-adspsvc.so \
    libOmxH264Dec.so \
    libOmxMpeg4Dec.so \
    libOmxVidEnc.so \
    akmd2 \
    copybit.msm7k \
    gralloc.samsung \
    libGLES_qcom.so \
    libjni_flashwidget


# Check : building for chinese people ?
ifdef CHINESE
	PRODUCT_PACKAGES += \
	PinyinIME
else
	PRODUCT_PACKAGES += \
	LatinIME
endif

# Check if we build with google or not
ifndef WITH_GOOGLE
	PRODUCT_PACKAGES += \
	    Provision \
	    GoogleSearch
endif

# Locales
PRODUCT_LOCALES := \
en_GB \
ar_EG \
ar_IL \
bg_BG \
ca_ES \
cs_CZ \
da_DK \
de_AT \
de_CH \
de_DE \
de_LI \
el_GR \
en_AU \
en_CA \
en_IE \
en_IN \
en_NZ \
en_SG \
en_US \
en_ZA \
es_ES \
es_US \
fi_FI \
fr_BE \
fr_CA \
fr_CH \
fr_FR \
he_IL \
hi_IN \
hr_HR \
hu_HU \
id_ID \
it_CH \
it_IT \
iw_IL \
ja_JP \
ko_KR \
lt_LT \
lv_LV \
nb_NO \
nl_BE \
nl_NL \
pl_PL \
pt_BR \
pt_PT \
ro_RO \
ru_RU \
sk_SK \
sl_SI \
sr_RS \
sv_SE \
th_TH \
tl_PH \
tr_TR \
uk_UA \
vi_VN \
zh_CN \
zh_TW
 
$(call inherit-product, build/target/product/full_base.mk)  

# Overrides
PRODUCT_BRAND := Samsung
PRODUCT_NAME := I7500
PRODUCT_DEVICE := I7500
PRODUCT_BOARD := I7500
PRODUCT_MANUFACTURER := Samsung
BUILD_ID := PUBLIC_BETA1

