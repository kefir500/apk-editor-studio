#!/bin/bash

# Prepare

cd "$(dirname "$BASH_SOURCE")"
VERSION=$(cat ../../../../VERSION)

# Build

cmake ../../../.. -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build || exit
cmake --install build --prefix=appdir/usr || exit

# Deploy

cp /usr/bin/adb appdir/usr/bin/
cp /usr/bin/zipalign appdir/usr/bin/
ln -sf appdir/usr/share/icons/hicolor/256x256/apps/apk-editor-studio.png appdir/apk-editor-studio.png

# Package

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage \
    appdir/usr/share/applications/apk-editor-studio.desktop \
    -appimage \
    -executable=appdir/usr/bin/adb \
    -executable=appdir/usr/bin/zipalign || exit
mv APK_Editor_Studio*.AppImage apk-editor-studio_linux_$VERSION.AppImage
