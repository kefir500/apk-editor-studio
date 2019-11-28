#!/bin/bash

# Prepare

cd "$(dirname "$0")"
if [ "$CI" != true ]; then source ../../environment.sh; fi

# Build

DESTDIR=apk-editor-studio_$VERSION
qmake ../../../.. DESTDIR="$DESTDIR"
make
make clean

# Package

tar cfJ apk-editor-studio_linux_$VERSION.tar.xz $DESTDIR
rm -rf $DESTDIR