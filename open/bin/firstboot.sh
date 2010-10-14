#!/system/bin/sh

# System default settings
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('trackball_wake_screen', 1);" 2&>/dev/null
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('trackball_unlock_screen', 1);" 2&>/dev/null
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO system (name, value) VALUES ('lock_home_in_memory', 1);" 2&>/dev/null
# Secure default settings
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO secure (name, value) VALUES ('install_non_market_apps', 1);" 2&>/dev/null
/system/xbin/sqlite3 /dbdata/databases/com.android.providers.settings/settings.db "INSERT INTO secure (name, value) VALUES ('allow_move_all_apps_external', 1);" 2&>/dev/null
