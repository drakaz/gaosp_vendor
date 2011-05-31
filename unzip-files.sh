#!/bin/sh


if [ -z "$1" ]; then
TAR="I7500-PDA.tar"
else
TAR=$1
fi
ZIP="I7500-PDA.zip"
ZIP2="CM.zip"

mkdir -p tmp/system/
mkdir proprietary
mkdir proprietary/modules
mkdir proprietary/bin
mkdir proprietary/keychars
mkdir proprietary/lib
mkdir proprietary/lib/hw

if [ `uname -m` = "x86_64" ]
then
	UNYAFFS_BIN=unyaffs64
else
	UNYAFFS_BIN=unyaffs32
fi
	
cd tmp/system/ && tar -xvf ../../$TAR system -C . && ../../$UNYAFFS_BIN system && rm -rf system && cd ../ && zip -r ../$ZIP system && cd ../ && rm -rf tmp/
unzip -j -o $ZIP system/bin/vold -d proprietary/bin/
unzip -j -o $ZIP system/bin/BCM4325D1* -d proprietary/bin/
unzip -j -o $ZIP system/lib/libsec-ril.so -d proprietary/lib/
unzip -j -o $ZIP system/lib/hw/copybit.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/etc/rtecdc.bin -d proprietary/etc/
unzip -j -o $ZIP system/usr/keychars/I7500_keypad.kcm.bin -d proprietary/keychars/

# Windows Media
unzip -j -o $ZIP2 system/lib/libomx_wmadec_sharedlibrary.so -d proprietary/lib/
unzip -j -o $ZIP2 system/lib/libomx_wmvdec_sharedlibrary.so -d proprietary/lib/
unzip -j -o $ZIP2 system/lib/libpvasfcommon.so -d proprietary/lib/
unzip -j -o $ZIP2 system/lib/libpvasflocalpbreg.so -d proprietary/lib/
unzip -j -o $ZIP2 system/lib/libpvasflocalpb.so -d proprietary/lib/
unzip -j -o $ZIP2 system/etc/pvasflocal.cfg -d proprietary/

# I7500 Proprietary lib

unzip -j -o $ZIP system/lib/libhgl.so system/bin/set_grp_id -d proprietary/lib/
unzip -j -o $ZIP system/lib/libaudioeq.so -d proprietary/lib/
unzip -j -o $ZIP  system/lib/libmmcamera.so system/lib/libmm-qcamera-tgt.so system/lib/libmmjpeg.so -d proprietary/lib/
sed -i "s|msm_camera0|msm_comera0|" proprietary/lib/libmm-qcamera-tgt.so
unzip -j -o $ZIP2 system/lib/libmm-adspsvc.so system/lib/libOmxH264Dec.so system/lib/libOmxMpeg4Dec.so system/lib/libOmxVidEnc.so -d proprietary/lib/
