#! /bin/bash
curl http://android.git.kernel.org/repo > repo
chmod 755 repo
./repo init -u git://github.com/drakaz/android.git -b gaosp-gingerbread
./repo sync
