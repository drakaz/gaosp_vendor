#!/bin/sh

mkdir -p proprietary/etc
wget -O proprietary/etc/rtecdc.bin http://obihoernchen.androidcodex.com/wifi_firmware/BCM4325_apsta.bin
mkdir -p proprietary/lib/egl
wget -O proprietary/lib/egl/libGLES_qcom.so http://obihoernchen.androidcodex.com/gaosp/libGLES_qcom.so
