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
                    device/Samsung/I7500/media_profiles.xml:/system/etc/media_profiles.xml \
                    device/Samsung/I7500/open/bin/rmmod:system/bin/rmmod


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
    Contacts \
    ContactsProvider \
    CustomLocale \
    DefaultContainerService \
    DownloadProvider \
    DeskClock \
    DSPManager \
    Email \
    Gallery3D \
    GlobalSearch \
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
    Provision \
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

ifdef CHINESE
PRODUCT_PACKAGES += \
    PinyinIME 
else
PRODUCT_PACKAGES += \
    LatinIME 
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
 
 
