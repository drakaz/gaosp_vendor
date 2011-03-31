WITH_GOOGLE := true

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=GRH78C BUILD_DISPLAY_ID=GRH78 BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2.1/FRG83D/75603:user/release-keys
PRIVATE_BUILD_DESC="passion-user 2.2.1 FRG83D 75603 release-keys"
DEVICE_PACKAGE_OVERLAYS := device/samsung/I7500/overlay

PACKAGES.Email.OVERRIDES := Provision LatinIME QuickSearchBox

# Used by BusyBox
KERNEL_MODULES_DIR:=/system/lib/modules

#PRODUCT_SPECIFIC_DEFINES += TARGET_PREBUILT_KERNEL=
#PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_DIR=gaosp-kernel
#PRODUCT_SPECIFIC_DEFINES += TARGET_KERNEL_CONFIG=gaosp_msm_defconfig
#LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)

# Tiny toolbox
TINY_TOOLBOX:=false

# Enable Windows Media if supported by the board
WITH_WINDOWS_MEDIA:=true

NO_DEFAULT_SOUNDS := true
PRODUCT_POLICY := android.policy_phone

#Use v8 Javascript engine
JS_ENGINE := v8

# PROPERTIES
PRODUCT_PROPERTY_OVERRIDES := \
    keyguard.no_require_sim=true \
    dalvik.vm.dexopt-data-only=0 \
    ro.ril.hsxpa=2 \
    ro.ril.gprsclass=10 \
    ro.media.dec.jpeg.memcap=10000000 \
    ro.config.ringtone=Playa.ogg \
    ro.config.notification_sound=regulus.ogg \
    ro.config.alarm_alert=Alarm_Beep_03.ogg \
    ro.sf.lcd_density=160 \
    ro.telephony.default_network=0 \
    dalvik.vm.execution-mode=int:fast \
    ro.opengles.version=65537 \
    ro.url.legal=http://www.google.com/intl/%s/mobile/android/basic/phone-legal.html \
    ro.url.legal.android_privacy=http://www.google.com/intl/%s/mobile/android/basic/privacy.html \
    ro.com.google.clientidbase=android-google \
    ro.com.android.wifi-watchlist=GoogleGuest \
    ro.setupwizard.enterprise_mode=1 \
    ro.com.android.dateformat=MM-dd-yyyy \
    ro.com.android.dataroaming=false \
    net.bt.name=Galaxy \
    debug.sf.hw=1 \
    video.accelerate.hw=1 \
    debug.performance.tuning=1

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
	device/samsung/I7500/open/etc/init.local.rc:system/etc/init.local.rc \
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
	device/samsung/I7500/open/modules/zram.ko:system/lib/modules/zram.ko \
	device/samsung/I7500/open/lib/librun.so:system/lib/librun.so \
	device/samsung/I7500/open/etc/init.d/00banner:system/etc/init.d/00banner \
	device/samsung/I7500/open/etc/init.d/01sysctl:system/etc/init.d/01sysctl \
	device/samsung/I7500/open/etc/init.d/03firstboot:system/etc/init.d/03firstboot \
	device/samsung/I7500/open/etc/init.d/04modules:system/etc/init.d/04modules \
	device/samsung/I7500/open/etc/init.d/20userinit:system/etc/init.d/20userinit \
	device/samsung/I7500/proprietary/etc/rtecdc.bin:system/etc/rtecdc.bin \
	device/samsung/I7500/open/etc/rtecdc_adhoc.bin:system/etc/rtecdc.bin \
	device/samsung/I7500/open/boot/ueventd.samsung.rc:root/ueventd.samsung.rc \
	device/samsung/I7500/proprietary/lib/set_grp_id:system/bin/set_grp_id \
	device/samsung/I7500/proprietary/lib/libaudioeq.so:system/lib/libaudioeq.so \
	device/samsung/I7500/proprietary/lib/libmm-qcamera-tgt.so:system/lib/libmm-qcamera-tgt.so \
	device/samsung/I7500/proprietary/lib/libmmjpeg.so:system/lib/libmmjpeg.so \
	device/samsung/I7500/proprietary/lib/libmmcamera.so:system/lib/libmmcamera.so \
	device/samsung/I7500/proprietary/lib/libOmxH264Dec.so:system/lib/libOmxH264Dec.so \
	device/samsung/I7500/proprietary/lib/libmm-adspsvc.so:system/lib/libmm-adspsvc.so \
	device/samsung/I7500/proprietary/lib/libOmxMpeg4Dec.so:system/lib/libOmxMpeg4Dec.so \
	device/samsung/I7500/proprietary/lib/libOmxVidEnc.so:system/lib/libOmxVidEnc.so \
	device/samsung/I7500/proprietary/lib/egl/libGLES_qcom.so:system/lib/egl/libGLES_qcom.so

# Packages
PRODUCT_PACKAGES := \
    bouncycastle \
    com.android.location.provider \
    com.android.location.provider.xml \
    core \
    core-junit \
    create_test_dmtrace \
    dalvikvm \
    dexdeps \
    dexdump \
    dexlist \
    dexopt \
    dmtracedump \
    dvz \
    dx \
    ext \
    hprof-conv \
    icu.dat \
    jasmin \
    jasmin.jar \
    libcrypto \
    libdex \
    libdvm \
    libexpat \
    libicui18n \
    libicuuc \
    libjavacore \
    libcyanogen-dsp \
    libnativehelper \
    libnfc_ndef \
    libsqlite_jni \
    libssl \
    libz \
    openvpn \
    sqlite-jdbc \
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
    DownloadProviderUi \
    DeskClock \
    DrmProvider \
    DSPManager \
    Email \
    FileManager \
    Gallery \
    Gallery3D \
    GaospConf \
    GoogleContactsProvider \
    GoogleSearch \
    Home \
    HTMLViewer \
    IM \
    ImProvider \
    klogtail \
    LatinIME \
    Launcher2 \
    LiveWallpapersPicker \
    ManagementProvider \
    MediaProvider \
    Mms \
    Music \
    PackageInstaller \
    Pacman \
    Phone \
    PicoTts \
    Protips \
    QuickSearchBox \
    Settings \
    SettingsProvider \
    SoundRecoder \
    SubscribedFeedsProvider \
    Superuser \
    Sync \
    SyncProvider \
    SystemUI \
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
    Torch \
    akmd2 \
    sensors.msm7k \
    copybit.msm7k \
    gralloc.msm7k \
    gps.msm7k \
    lights.msm7k \
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
    libGLES_qcom.so \
    libjni_flash \
    libjni_flashwidget


# Check : building for chinese people ?
ifdef CHINESE
	PRODUCT_PACKAGES += \
	PinyinIME
else
ifdef JAPENESE
	PRODUCT_PACKAGES += \
	OpenWnn \
    	libWnnEngDic \
    	libWnnJpnDic \
    	libwnndict
else
	PRODUCT_PACKAGES += \
	LatinIME
endif
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
 

# Pick up some sounds - stick with the short list to save space
# on smaller devices.
$(call inherit-product-if-exists, frameworks/base/data/sounds/OriginalAudio.mk)

# Get the TTS language packs
$(call inherit-product-if-exists, external/svox/pico/lang/all_pico_languages.mk)

# Get the list of languages.
$(call inherit-product, $(SRC_TARGET_DIR)/product/languages_full.mk)

# Overrides
PRODUCT_BRAND := Samsung
PRODUCT_NAME := I7500
PRODUCT_DEVICE := I7500
PRODUCT_BOARD := I7500
PRODUCT_MANUFACTURER := Samsung
BUILD_ID := PUBLIC_BETA1

