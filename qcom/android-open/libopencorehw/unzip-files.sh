#!/bin/sh

mkdir -p proprietary

unzip -j -o ../../../$1 system/lib/libopencorehw.so -d proprietary
touch proprietary/*
