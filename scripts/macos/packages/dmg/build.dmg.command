#!/bin/bash

# Prepare

cd "$(dirname "$0")"
if [ "$CI" != true ]; then source ../../environment.sh; fi

# Package

rm "apk-editor-studio_$VERSION.dmg"
npx appdmg "appdmg/config.json" "apk-editor-studio_macos_$VERSION.dmg" || { echo "Could not create DMG."; exit 1; }
