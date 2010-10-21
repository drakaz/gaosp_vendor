#!/system/bin/sh

# Wait for settings.db to be created
while [ ! -e /dbdata/databases/com.android.providers.settings/settings.db ]
do
	sleep 1
done
sleep 5
# System default settings
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('trackball_wake_screen', 1);" 
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('trackball_unlock_screen', 1);" 
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('lock_home_in_memory', 1);"
# Secure default settings
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO secure (name, value) VALUES ('install_non_market_apps', 1);"
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO secure (name, value) VALUES ('allow_move_all_apps_external', 1);"
