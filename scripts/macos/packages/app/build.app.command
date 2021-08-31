#!/bin/bash

# Prepare

cd "$(dirname "$BASH_SOURCE")"
VERSION=$(cat ../../../../VERSION)

# Build & Deploy

cmake ../../../.. -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release || exit

# Package

mv build/apk-editor-studio.app "APK Editor Studio.app"
ditto -c -k --sequesterRsrc --keepParent "APK Editor Studio.app" "apk-editor-studio_macos_$VERSION.app.zip"
