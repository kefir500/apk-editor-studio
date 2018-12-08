#!/bin/bash

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh
rm -rf apk-editor-studio.app
rm -rf "APK Editor Studio.app"

# Build & Deploy

~/Qt5.*/5.*/clang_64/bin/qmake ../../../.. DESTDIR=. && make || { echo "Could not build the project."; exit 1; }
~/Qt5.*/5.*/clang_64/bin/macdeployqt apk-editor-studio.app || { echo "Could not deploy the project."; exit 2; }
find apk-editor-studio.app -name ".DS_Store" -type f -delete

# Package

mv apk-editor-studio.app "APK Editor Studio.app"
ditto -c -k --sequesterRsrc --keepParent "APK Editor Studio.app" "apk-editor-studio_$VERSION.app.zip"

# Clean

make clean
