#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../$1 system/lib/libhgl.so system/bin/set_grp_id -d proprietary
touch proprietary/*

chmod 755 proprietary/set_grp_id
