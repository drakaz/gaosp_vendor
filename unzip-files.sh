#!/bin/sh


if [ -z "$1" ]; then
TAR="I7500-PDA.tar"
else
TAR=$1
fi
ZIP="I7500-PDA.zip"

mkdir -p tmp/system/
mkdir proprietary
mkdir proprietary/modules
mkdir proprietary/bin
mkdir proprietary/lib
mkdir proprietary/lib/hw


cd tmp/system/ && tar -xvf ../../$TAR system -C . && ../../unyaffs system && rm system && cd ../ && zip -r ../$ZIP system && cd ../ && rm -rf tmp/
unzip -j -o $ZIP system/libmodules/multipdp.ko -d proprietary/modules/
unzip -j -o $ZIP system/bin/akmd2 -d proprietary/bin/
unzip -j -o $ZIP system/bin/vold -d proprietary/bin/
unzip -j -o $ZIP system/lib/libsec-ril.so -d proprietary/lib/
unzip -j -o $ZIP system/lib/hw/copybit.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/lib/hw/lights.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/lib/hw/sensors.samsung.so -d proprietary/lib/hw/
unzip -j -o $ZIP system/etc/rtecdc.bin -d proprietary/etc/

(cd qcom/android-open/libopencorehw && ./unzip-files.sh $ZIP)
(cd qcom/android-open/loc_api && ./unzip-files.sh $ZIP)
(cd qcom/android-open/pvomx && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/dsp/libqcamera && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/dsp/mm && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libaudioeq && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libgps && ./unzip-files.sh $ZIP)
(cd qcom/proprietary-open/libhgl && ./unzip-files.sh $ZIP)
