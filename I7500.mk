# Overrides
PRODUCT_BRAND := I7500
PRODUCT_NAME := I7500
PRODUCT_DEVICE := I7500
PRODUCT_MANUFACTURER := Samsung
BUILD_ID := PUBLIC_BETA3

DEVICE_PACKAGE_OVERLAYS := device/samsung/I7500/overlay

# Enable root on userdebug
ADDITIONAL_DEFAULT_PROPERTIES += ro.secure=0

NO_DEFAULT_SOUNDS := true
PRODUCT_POLICY := android.policy_phone
PRODUCT_PROPERTY_OVERRIDES := \
    ro.config.ringtone=DonMessWivIt.ogg \
    ro.config.notification_sound=pixiedust.ogg \
    ro.config.alarm_alert=Alarm_Classic.ogg \
    keyguard.no_require_sim=true \
    ro.com.android.dateformat=MM-dd-yyyy \
    dalvik.vm.dexopt-data-only=0 \
    ro.com.android.wifi-watchlist=GoogleGuest \
    ro.setupwizard.enterprise_mode=1 \
    ro.com.android.dataroaming=false \
    ro.url.legal=http://www.google.com/intl/%s/mobile/android/basic/phone-legal.html \
    ro.url.legal.android_privacy=http://www.google.com/intl/%s/mobile/android/basic/privacy.html \
    ro.com.google.clientidbase=android-google \
    ro.setupwizard.mode=OPTIONAL \
    net.bt.name=Android \
    ro.com.google.locationfeatures=1 \
    ro.com.google.gmsversion=2.1_r1 \
    ro.config.sync=yes \
    dalvik.vm.stack-trace-file=/data/anr/traces.txt \
    ro.media.dec.aud.wma.enabled=1 \
    ro.media.dec.vid.wmv.enabled=1 \
    ro.media.dec.jpeg.memcap=10000000 \
    wifi.supplicant_scan_interval=15 \
    ro.telephony.default_network=0 \
    ro.sf.lcd_density=160 \
    # dalvik.vm.execution-mode=int:fast

# Other Files
PRODUCT_COPY_FILES += device/Samsung/I7500/open/etc/apns-conf.xml:system/etc/apns-conf.xml \
                    device/Samsung/I7500/proprietary/bin/vold:system/bin/vold \
		    frameworks/base/data/etc/handheld_core_hardware.xml:system/etc/permissions/handheld_core_hardware.xml \
		    frameworks/base/data/etc/android.hardware.camera.autofocus.xml:system/etc/permissions/android.hardware.camera.autofocus.xml \
		    frameworks/base/data/etc/android.hardware.telephony.gsm.xml:system/etc/permissions/android.hardware.telephony.gsm.xml \
		    frameworks/base/data/etc/android.hardware.location.gps.xml:system/etc/permissions/android.hardware.location.gps.xml \
		    frameworks/base/data/etc/android.hardware.wifi.xml:system/etc/permissions/android.hardware.wifi.xml \
		    frameworks/base/data/etc/android.hardware.touchscreen.multitouch.xml:system/etc/permissions/android.hardware.touchscreen.multitouch.xml \
	            device/Samsung/I7500/media_profiles.xml:/system/etc/media_profiles.xml

# Sounds
include frameworks/base/data/sounds/AudioPackage4.mk
# TTS languages
#include external/svox/pico/lang/PicoLangDeDeInSystem.mk
#include external/svox/pico/lang/PicoLangEnGBInSystem.mk
#include external/svox/pico/lang/PicoLangEnUsInSystem.mk
#include external/svox/pico/lang/PicoLangEsEsInSystem.mk
#include external/svox/pico/lang/PicoLangFrFrInSystem.mk
#include external/svox/pico/lang/PicoLangItItInSystem.mk

ifeq ($(TARGET_PREBUILT_KERNEL),)
LOCAL_KERNEL := device/samsung/I7500/open/boot/kernel
else
LOCAL_KERNEL := $(TARGET_PREBUILT_KERNEL)
endif

# Packages
PRODUCT_PACKAGES := \
    libRS \
    librs_jni \
    framework-res \
    AlarmClock \
    AlarmProvider \
    AccountAndSyncSettings \
    ApplicationsProvider \
    ADWLauncher \
    Bluetooth \
    Browser \
    Bugreport \
    Calculator \
    Calendar \
    CalendarProvider \
    Camera \
    CertInstaller \
    Contacts \
    ContactsProvider \
    CustomLocale \
    DownloadProvider \
    DeskClock \
    Email \
    Gallery3D \
    GlobalSearch \
    GoogleSearch \
    HTMLViewer \
    IM \
    ImProvider \
    LatinIME \
    ManagementProvider \
    MediaProvider \
    Mms \
    Music \
    PackageInstaller \
    Phone \
    PicoTts \
    Provision \
    Settings \
    SettingsProvider \
    SoundRecoder \
    SubscribedFeedsProvider \
    Stk \
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
 
 
