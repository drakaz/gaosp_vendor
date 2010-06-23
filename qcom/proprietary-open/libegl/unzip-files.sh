#!/bin/sh

mkdir -p proprietary
unzip -j -o ../../../$1 system/lib/egl/libGLES_android.so -d proprietary
unzip -j -o ../../../$1 system/lib/egl/libGLES_qcom.so -d proprietary
touch proprietary/*
