#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../$1 system/lib/libgps.so -d proprietary
touch proprietary/*
