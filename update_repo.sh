#! /bin/bash
cd .repo
rm -f local_manifest.xml
wget http://gaosp.googlecode.com/files/local_manifest.xml
cd ..
repo sync
