#!/bin/bash

cd "$(dirname "$0")"
if [ -z "$VERSION" ]; then VERSION=$(cat ../../../../VERSION); fi
cp -R ../../../../bin/linux apk-editor-studio_linux_$VERSION
tar cfJ apk-editor-studio_linux_$VERSION.tar.xz apk-editor-studio_$VERSION
rm -rf apk-editor-studio_$VERSION
