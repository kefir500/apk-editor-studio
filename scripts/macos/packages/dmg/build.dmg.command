#!/bin/bash

# Prepare

cd "$(dirname "$0")"
source ../../environment.sh

# Package

rm "apk-editor-studio_$VERSION.dmg"
appdmg "appdmg/config.json" "apk-editor-studio_macos_$VERSION.dmg" || { echo "Could not create DMG."; exit 1; }
