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
unzip -j -o $ZIP system/bin/akmd2 -d proprietary/bin/
unzip -j -o $ZIP system/bin/vold -d proprietary/bin/
unzip -j -o $ZIP system/bin/BCM4325D1* -d proprietary/bin/
unzip -j -o $ZIP system/lib/libsec-ril.so -d proprietary/lib/
unzip -j -o $ZIP system/lib/libloc_api.so -d proprietary/lib/
unzip -j -o $ZIP system/lib/hw/copybit.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/lib/hw/lights.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/etc/rtecdc.bin -d proprietary/etc/
unzip -j -o $ZIP system/usr/keychars/I7500_keypad.kcm.bin -d proprietary/keychars/

# Windows Media
unzip -j -o $ZIP2 system/lib/libomx_wmadec_sharedlibrary.so proprietary/lib/libomx_wmadec_sharedlibrary.so
unzip -j -o $ZIP2 system/lib/libomx_wmvdec_sharedlibrary.so proprietary/lib/libomx_wmvdec_sharedlibrary.so
unzip -j -o $ZIP2 system/lib/libpvasfcommon.so proprietary/lib/libpvasfcommon.so
unzip -j -o $ZIP2 system/lib/libpvasflocalpbreg.so proprietary/lib/libpvasflocalpbreg.so
unzip -j -o $ZIP2 system/lib/libpvasflocalpb.so proprietary/lib/libpvasflocalpb.so
unzip -j -o $ZIP2 system/etc/pvasflocal.cfg proprietary/pvasflocal.cfg

(cd qcom/proprietary-open/loc_api && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/dsp/libqcamera && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/dsp/mm && ./unzip-files.sh $ZIP2)
(cd qcom/proprietary-open/libaudioeq && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libgps && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libhgl && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libegl && ./unzip-files.sh $ZIP2)
