#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../../$1 system/lib/libmm-adspsvc.so system/lib/libOmxH264Dec.so system/lib/libOmxMpeg4Dec.so system/lib/libOmxVidEnc.so -d proprietary
touch proprietary/*
