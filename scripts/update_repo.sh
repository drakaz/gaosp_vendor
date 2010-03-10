#! /bin/bash
curl http://android.git.kernel.org/repo > repo
chmod 755 repo
cd .repo
rm -f local_manifest.xml
wget http://gaosp.googlecode.com/files/local_manifest.xml
cd ..
./repo sync
