#!/bin/bash

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh
rm -rf apk-editor-studio.app
rm -rf "APK Editor Studio.app"

# Build & Deploy

qmake ../../../.. QMAKE_CXXFLAGS+=-Wno-inconsistent-missing-override DESTDIR=. && make || { echo "Could not build the project."; exit 1; }
macdeployqt apk-editor-studio.app || { echo "Could not deploy the project."; exit 2; }
find apk-editor-studio.app -name ".DS_Store" -type f -delete

# Package

mv apk-editor-studio.app "APK Editor Studio.app"
ditto -c -k --sequesterRsrc --keepParent "APK Editor Studio.app" "apk-editor-studio_macos_$VERSION.app.zip"

# Clean

make clean
