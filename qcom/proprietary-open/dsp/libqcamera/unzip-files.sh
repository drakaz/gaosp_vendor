#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../../$1 system/lib/libcamera.so system/lib/libcameraservice.so -d proprietary
touch proprietary/*
