#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../$1 system/lib/libloc_api.so -d proprietary
touch proprietary/*
