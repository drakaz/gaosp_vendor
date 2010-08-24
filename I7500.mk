# Overrides
PRODUCT_BRAND := Samsung
PRODUCT_NAME := I7500
PRODUCT_DEVICE := I7500
PRODUCT_BOARD := I7500
PRODUCT_MANUFACTURER := Samsung
BUILD_ID := PUBLIC_BETA3

PRODUCT_BUILD_PROP_OVERRIDES += BUILD_ID=FRF91 BUILD_DISPLAY_ID=FRF91 PRODUCT_NAME=passion BUILD_FINGERPRINT=google/passion/passion/mahimahi:2.2/FRF91/43546:user/release-keys 
PRIVATE_BUILD_DESC="galaxy-user 2.2 FRF91 43546 release-keys"

DEVICE_PACKAGE_OVERLAYS := device/Samsung/I7500/overlay

PACKAGES.Email.OVERRIDES := Provision LatinIME QuickSearchBox

# Used by BusyBox
KERNEL_MODULES_DIR:=/system/lib/modules

# Tiny toolbox
TINY_TOOLBOX:=true

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
    ro.ril.hsxpa=1 \
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
    net.bt.name=Galaxy \

# Disable Compcache by default
PRODUCT_PROPERTY_OVERRIDES += \
    ro.compcache.default=0

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
	device/Samsung/I7500/open/etc/media_profiles.xml:/system/etc/media_profiles.xml \
	device/Samsung/I7500/open/etc/resolv.conf:/system/etc/resolv.conf \
	device/Samsung/I7500/open/etc/sysctl.conf:/system/etc/sysctl.conf \
	device/Samsung/I7500/open/etc/terminfo/l/linux:/etc/terminfo/l/linux \
	device/Samsung/I7500/open/etc/terminfo/u/unknown:/etc/terminfo/u/unknown \
	device/Samsung/I7500/open/etc/fstab:/etc/fstab \
	device/Samsung/I7500/open/etc/bash.bashrc:/etc/bash.bashrc \
	device/Samsung/I7500/open/etc/gshadow:/etc/gshadow \
	device/Samsung/I7500/open/etc/passwd:/etc/passwd \
	device/Samsung/I7500/open/etc/profile:/etc/profile \
	device/Samsung/I7500/open/etc/shadow:/etc/shadow \
	device/Samsung/I7500/open/etc/group:/etc/group \
	device/Samsung/I7500/open/etc/shells:/etc/shells \
	device/Samsung/I7500/open/etc/zshrc:/etc/zshrc \
	device/Samsung/I7500/open/etc/adb_shell.conf:/etc/adb_shell.conf \
	device/Samsung/I7500/open/bin/rc:/bin/rc \
	device/Samsung/I7500/open/bin/compcache:/bin/compcache \
	device/Samsung/I7500/open/bin/handle_compcache:/bin/handle_compcache \
	device/Samsung/I7500/open/bin/fix_permissions:/bin/fix_permissions \
	device/Samsung/I7500/open/bin/openvpn-up.sh:/xbin/openvpn-up.sh \
	device/Samsung/I7500/open/bin/htop:/xbin/htop \
	device/Samsung/I7500/open/bin/irssi:/xbin/irssi \
	device/Samsung/I7500/open/bin/lsof:/xbin/lsof \
	device/Samsung/I7500/open/bin/powertop:/xbin/powertop \
	device/Samsung/I7500/open/bin/shutdown:/xbin/shutdown \
	device/Samsung/I7500/open/bin/voldupdater:/xbin/voldupdater \
	device/Samsung/I7500/open/bin/remountro:/xbin/remountro \
	device/Samsung/I7500/open/bin/zsh:/xbin/zsh \
	device/Samsung/I7500/open/bin/bash:/bin/bash \
	device/Samsung/I7500/open/bin/sysinit:/bin/sysinit \
	device/Samsung/I7500/open/bin/fbvncserver:/bin/fbvncserver \
	device/Samsung/I7500/open/bin/akmd2:/bin/akmd2 \
	device/Samsung/I7500/open/bin/BCM4325D1_004.002.004.0153.0156.hcd:/bin/BCM4325D1_004.002.004.0153.0156.hcd \
	device/Samsung/I7500/open/apps/GalaxoParameters.apk:/app/GalaxoParameters.apk \
	device/Samsung/I7500/open/apps/GaospConf.apk:/app/GaospConf.apk \
	device/Samsung/I7500/open/boot/bootanimation.zip:/media/bootanimation.zip \
	device/Samsung/I7500/open/boot/logo.rle:/logo.rle \
	device/Samsung/I7500/open/modules/ramzswap.ko:/lib/modules/ramzswap.ko \
	device/Samsung/I7500/open/lib/librun.so:/lib/librun.so \
	device/Samsung/I7500/open/lib/libncurses.so:/lib/libncurses.so \
	device/Samsung/I7500/open/etc/init.d/00banner:/etc/init.d/00banner \
	device/Samsung/I7500/open/etc/init.d/01sysctl:/etc/init.d/01sysctl \
	device/Samsung/I7500/open/etc/init.d/03firstboot:/etc/init.d/03firstboot \
	device/Samsung/I7500/open/etc/init.d/04modules:/etc/init.d/04modules \
	device/Samsung/I7500/open/etc/init.d/20userinit:/etc/init.d/20userinit
	

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
    Superuser


ifdef WITH_GOOGLE
	ifdef CHINESE
	PRODUCT_PACKAGES += \
	    PinyinIME 
	endif
else
	ifdef CHINESE
	PRODUCT_PACKAGES += \
	    PinyinIME 
	else
	PRODUCT_PACKAGES += \
	    LatinIME 
	endif

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
 
 
