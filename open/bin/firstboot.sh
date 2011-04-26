#!/system/bin/sh

DATABASE="/data/data/com.android.providers.settings/databases/settings.db"

# Wait for settings.db to be created
echo "firstboot.sh : waiting for system boot done"
getprop sys.boot_completed
while [ `getprop sys.boot_completed` = 0 ]
do
	sleep 1
done
echo "firstboot.sh : waiting for $DATABASE"
while [ ! -e $DATABASE ]
do
	sleep 1
done
sleep 5
echo "firstboot.sh : database available, let's go"
# System default settings
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('trackball_wake_screen', 1);" 
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('trackball_unlock_screen', 1);" 
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lock_home_in_memory', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lock_mms_in_memory', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lockscreen_rotary_unlock_down', 0);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lockscreen_rotary_hide_arrows', 0);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lockscreen_custom_app_toggle', 0);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lockscreen_custom_icon_style', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('lockscreen_style_pref', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO system (name, value) VALUES ('screen_brightness_mode', 1);"
# Secure default settings
/system/xbin/sqlite3 $DATABASE "INSERT INTO secure (name, value) VALUES ('device_provisioned', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO secure (name, value) VALUES ('install_non_market_apps', 1);"
/system/xbin/sqlite3 $DATABASE "INSERT INTO secure (name, value) VALUES ('allow_move_all_apps_external', 1);"

# Persist sysprop
echo 1 > /data/property/persist.sys.purgeable_assets
echo 0 > /data/property/persist.sys.use_dithering
