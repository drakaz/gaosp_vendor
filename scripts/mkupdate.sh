#! /bin/bash
rm -rf out/target/product/I7500/unsigned.zip
rm -rf out/target/product/I7500/update.zip
#rm -rf out/target/product/I7500/boot.img
cp -r vendor/Samsung/I7500/META-INF out/target/product/I7500/

### For future kernel generation, for now we just get prebuild boot.img from gcode download page
#rm -rf out/target/product/I7500/ramdisk.gz
#cp -rp vendor/Samsung/I7500/open/boot/init out/target/product/I7500/root/init
#out/host/linux-x86/bin/mkbootfs out/target/product/I7500/root/ | gzip > out/target/product/I7500/ramdisk.gz
#out/host/linux-x86/bin/mkbootimg --kernel vendor/Samsung/I7500/open/boot/kernel --ramdisk out/target/product/I7500/ramdisk.gz -o out/target/product/I7500/boot.img
#wget -q http://gaosp.googlecode.com/files/boot.img -O out/target/product/I7500/boot.img
###

#find . -name \*~ -exec rm {} \;

### Remove symlinks, they are created in the install script
find  out/target/product/I7500/system/xbin/ -type l -exec rm "{}" \;
find  out/target/product/I7500/system/bin/ -type l -exec rm "{}" \;

cd out/target/product/I7500/

### zip and sign
zip -yr unsigned.zip META-INF data system boot.img
java -jar ../../../host/linux-x86/framework/signapk.jar ../../../../build/target/product/security/testkey.x509.pem ../../../../build/target/product/security/testkey.pk8 unsigned.zip update.zip
