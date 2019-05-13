#!/bin/bash

clean() {
    make clean
    rm -rf appdir 2> /dev/null
    rm .qmake.stash Makefile*
}

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh
source /opt/qt*/bin/qt*-env.sh
clean
mkdir appdir

# Build

if [[ $CI != true ]]; then
    qmake ../../../.. PREFIX="$(pwd)/appdir/usr"
    make
    make install
fi

# Package

wget -c -nv "https://github.com/probonopd/linuxdeployqt/releases/download/continuous/linuxdeployqt-continuous-x86_64.AppImage"
chmod +x linuxdeployqt-continuous-x86_64.AppImage
./linuxdeployqt-continuous-x86_64.AppImage appdir/usr/share/applications/apk-editor-studio.desktop -appimage
mv APK_Editor_Studio*.AppImage apk-editor-studio_linux_$VERSION.AppImage
clean
