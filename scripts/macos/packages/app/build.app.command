#!/bin/bash

# Prepare

cd "$(dirname "$BASH_SOURCE")"
VERSION=$(cat ../../../../VERSION)
rm -rf apk-editor-studio.app
rm -rf "APK Editor Studio.app"

# Build & Deploy

qmake ../../../.. QMAKE_CXXFLAGS+=-Wno-inconsistent-missing-override DESTDIR=.
make || exit
macdeployqt apk-editor-studio.app || exit
find apk-editor-studio.app -name ".DS_Store" -type f -delete

# Package

mv apk-editor-studio.app "APK Editor Studio.app"
ditto -c -k --sequesterRsrc --keepParent "APK Editor Studio.app" "apk-editor-studio_macos_$VERSION.app.zip"
make clean
