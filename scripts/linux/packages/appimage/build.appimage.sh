#!/bin/bash

clean() {
    make clean
    rm .qmake.stash Makefile*
    rm -rf appdir 2> /dev/null
}

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh
clean

# Build

qmake ../../../..
make
make install INSTALL_ROOT=appdir

# Package

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/share/applications/apk-editor-studio.desktop -appimage
if [[ $CI != true ]]; then
    mv APK_Editor_Studio*.AppImage apk-editor-studio_linux_$VERSION.AppImage
else
    mv APK_Editor_Studio*.AppImage apk-editor-studio_linux_dev.AppImage
fi
clean
