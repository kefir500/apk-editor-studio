#!/bin/bash

clean() {
    make clean
    rm .qmake.stash Makefile*
    rm -rf appdir 2> /dev/null
}

# Prepare

cd "$(dirname "$BASH_SOURCE")"
VERSION=$(cat ../../../../VERSION)
clean

# Build

qmake ../../../.. PACKAGE=appimage
make || exit
make install INSTALL_ROOT=appdir

# Deploy

cp /usr/bin/adb appdir/usr/bin/
cp /usr/bin/zipalign appdir/usr/bin/

# Package

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/share/applications/apk-editor-studio.desktop -appimage -executable=appdir/usr/bin/adb -executable=appdir/usr/bin/zipalign || exit
mv APK_Editor_Studio*.AppImage apk-editor-studio_linux_$VERSION.AppImage
clean
