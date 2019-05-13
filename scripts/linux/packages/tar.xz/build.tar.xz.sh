#!/bin/bash

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh

# Build

if [[ $CI != true ]]; then
    qmake ../../../..
    make
    make clean
fi

# Package

tar cfJ apk-editor-studio_linux_$VERSION.tar.xz ../../../../bin/linux --transform "s,^bin/linux,apk-editor-studio_$VERSION,"
