# Overrides
PRODUCT_BRAND := Samsung
PRODUCT_NAME := I7500
PRODUCT_DEVICE := I7500
PRODUCT_BOARD := I7500
PRODUCT_MANUFACTURER := Samsung
BUILD_ID := PUBLIC_BETA3

WITH_GOOGLE := true

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=FRG83 BUILD_DISPLAY_ID=FRG83 BUILD_FINGERPRINT=tmobile/opal/sapphire/sapphire:2.2.1/FRG83/60505:user/release-keys 
PRIVATE_BUILD_DESC="opal-user 2.2.1 FRG83 60505 release-keys"
DEVICE_PACKAGE_OVERLAYS := device/Samsung/I7500/overlay

PACKAGES.Email.OVERRIDES := Provision LatinIME QuickSearchBox

# Used by BusyBox
KERNEL_MODULES_DIR:=/system/lib/modules

# Tiny toolbox
TINY_TOOLBOX:=false

# Enable Windows Media if supported by the board
WITH_WINDOWS_MEDIA:=true

# Enable root on userdebug
ADDITIONAL_DEFAULT_PROPERTIES += ro.secure=0

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
        ro.modversion=GAOSP-2-$(shell date +%m%d%Y)-NIGHTLY
else
    PRODUCT_PROPERTY_OVERRIDES += \
        ro.modversion=GAOSP-2-PUBLIC-BETA-3
endif

# Other Files
PRODUCT_COPY_FILES += device/Samsung/I7500/open/etc/apns-conf.xml:system/etc/apns-conf.xml \
	frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
	frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
	frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
	frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
	frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
	frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
	device/Samsung/I7500/open/etc/media_profiles.xml:system/etc/media_profiles.xml \
	device/Samsung/I7500/open/etc/resolv.conf:system/etc/resolv.conf \
	device/Samsung/I7500/open/etc/sysctl.conf:system/etc/sysctl.conf \
	device/Samsung/I7500/open/etc/terminfo/l/linux:system/etc/terminfo/l/linux \
	device/Samsung/I7500/open/etc/terminfo/u/unknown:system/etc/terminfo/u/unknown \
	device/Samsung/I7500/open/etc/fstab:system/etc/fstab \
	device/Samsung/I7500/open/etc/bash.bashrc:system/etc/bash.bashrc \
	device/Samsung/I7500/open/etc/gshadow:system/etc/gshadow \
	device/Samsung/I7500/open/etc/passwd:system/etc/passwd \
	device/Samsung/I7500/open/etc/profile:system/etc/profile \
	device/Samsung/I7500/open/etc/shadow:system/etc/shadow \
	device/Samsung/I7500/open/etc/group:system/etc/group \
	device/Samsung/I7500/open/etc/shells:system/etc/shells \
	device/Samsung/I7500/open/etc/zshrc:system/etc/zshrc \
	device/Samsung/I7500/open/etc/adb_shell.conf:system/etc/adb_shell.conf \
	device/Samsung/I7500/open/etc/gaosp.conf:system/etc/gaosp.conf \
	device/Samsung/I7500/open/bin/rc:system/bin/rc \
	device/Samsung/I7500/open/bin/compcache:system/bin/compcache \
	device/Samsung/I7500/open/bin/handle_compcache:system/bin/handle_compcache \
	device/Samsung/I7500/open/bin/fix_permissions:system/bin/fix_permissions \
	device/Samsung/I7500/open/bin/openvpn-up.sh:system/xbin/openvpn-up.sh \
	device/Samsung/I7500/open/bin/htop:system/xbin/htop \
	device/Samsung/I7500/open/bin/irssi:system/xbin/irssi \
	device/Samsung/I7500/open/bin/lsof:system/xbin/lsof \
	device/Samsung/I7500/open/bin/powertop:system/xbin/powertop \
	device/Samsung/I7500/open/bin/shutdown:system/xbin/shutdown \
	device/Samsung/I7500/open/bin/voldupdater:system/xbin/voldupdater \
	device/Samsung/I7500/open/bin/remountro:system/xbin/remountro \
	device/Samsung/I7500/open/bin/remountrw:system/xbin/remountrw \
	device/Samsung/I7500/open/bin/zsh:system/xbin/zsh \
	device/Samsung/I7500/open/bin/bash:system/bin/bash \
	device/Samsung/I7500/open/bin/sysinit:system/bin/sysinit \
	device/Samsung/I7500/open/bin/fbvncserver:system/bin/fbvncserver \
	device/Samsung/I7500/open/bin/changepasswd:system/bin/changepasswd \
	device/Samsung/I7500/open/bin/dropbear:system/xbin/dropbear \
	device/Samsung/I7500/open/bin/dropbear-keygen:system/xbin/dropbear-keygen \
	device/Samsung/I7500/open/bin/busybox:system/xbin/busybox \
	device/Samsung/I7500/proprietary/bin/akmd2:system/bin/akmd2 \
	device/Samsung/I7500/proprietary/bin/BCM4325D1_004.002.004.0153.0156.hcd:system/bin/BCM4325D1_004.002.004.0153.0156.hcd \
	device/Samsung/I7500/open/apps/GaospConf.apk:system/app/GaospConf.apk \
	device/Samsung/I7500/open/apps/serviceModeApp.apk:system/app/serviceModeApp.apk \
	device/Samsung/I7500/open/apps/HotRebootFree.apk:system/app/HotRebootFree.apk \
	device/Samsung/I7500/open/boot/bootanimation.zip:system/media/bootanimation.zip \
	device/Samsung/I7500/open/boot/logo.rle:root/logo.rle \
	device/Samsung/I7500/open/modules/ramzswap.ko:system/lib/modules/ramzswap.ko \
	device/Samsung/I7500/open/lib/librun.so:system/lib/librun.so \
	device/Samsung/I7500/open/etc/init.d/00banner:system/etc/init.d/00banner \
	device/Samsung/I7500/open/etc/init.d/01sysctl:system/etc/init.d/01sysctl \
	device/Samsung/I7500/open/etc/init.d/03firstboot:system/etc/init.d/03firstboot \
	device/Samsung/I7500/open/etc/init.d/04modules:system/etc/init.d/04modules \
	device/Samsung/I7500/open/etc/init.d/20userinit:system/etc/init.d/20userinit \
        device/Samsung/I7500/open/etc/rtecdc_adhoc.bin:system/etc/rtecdc.bin
	

# Sounds
include frameworks/base/data/sounds/AudioPackage4.mk
# TTS languages
#include external/svox/pico/lang/PicoLangDeDeInSystem.mk
#include external/svox/pico/lang/PicoLangEnGBInSystem.mk
#include external/svox/pico/lang/PicoLangEnUsInSystem.mk
#include external/svox/pico/lang/PicoLangEsEsInSystem.mk
#include external/svox/pico/lang/PicoLangFrFrInSystem.mk
#include external/svox/pico/lang/PicoLangItItInSystem.mk

# Packages
PRODUCT_PACKAGES := \
    framework-res \
    AlarmClock \
    AlarmProvider \
    AccountAndSyncSettings \
    ADWLauncher \
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
    DefaultContainerService \
    DownloadProvider \
    DeskClock \
    DSPManager \
    Email \
    Gallery3D \
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
    Phone \
    PicoTts \
    Protips \
    QuickSearchBox \
    Settings \
    SettingsProvider \
    SoundRecoder \
    SubscribedFeedsProvider \
    Stk \
    SpeechRecorder \
    TelephonyProvider \
    Term \
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
    sensors.msm7k


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
zh_TW \
mdpi \
hdpi
 
 
