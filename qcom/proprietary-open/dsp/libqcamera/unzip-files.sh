#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../../$1 system/lib/libcamera.so system/lib/libmmcamera.so system/lib/libmm-qcamera-tgt.so system/lib/libmmjpeg.so -d proprietary
touch proprietary/*
