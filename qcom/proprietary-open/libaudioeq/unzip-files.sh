#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../$1 system/lib/libaudioeq.so -d proprietary
touch proprietary/*
