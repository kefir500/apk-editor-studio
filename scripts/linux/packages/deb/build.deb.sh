#!/bin/bash

cd "$(dirname "$0")"

source ../../environment.sh

clean() {
    rm -rf apk-editor-studio-$VERSION 2> /dev/null
    rm apk-editor-studio_$VERSION*.tar.xz apk-editor-studio_$VERSION*.dsc apk-editor-studio_$VERSION*.changes apk-editor-studio_$VERSION*.build 2> /dev/null
}

clean
rm apk-editor-studio_$VERSION*.deb 2> /dev/null
mkdir apk-editor-studio-$VERSION
cp ../../../../*.pro apk-editor-studio-$VERSION/
cp ../../../../*.pri apk-editor-studio-$VERSION/
cp -R ../../../../lib apk-editor-studio-$VERSION/
cp -R ../../../../res apk-editor-studio-$VERSION/
cp -R ../../../../src apk-editor-studio-$VERSION/
cp -R debian apk-editor-studio-$VERSION/
cd apk-editor-studio-$VERSION

tar -cJf ../apk-editor-studio_$VERSION.orig.tar.xz .
debuild -uc -us || exit

cd ..
if [[ $CI != true ]]; then	
    mv apk-editor-studio_$VERSION*.deb apk-editor-studio_linux_$VERSION.deb	
else	
    mv apk-editor-studio_$VERSION*.deb apk-editor-studio_linux_dev.deb	
fi
clean
