#!/bin/bash

# Prepare

cd "$(dirname "$BASH_SOURCE")"
VERSION=$(cat ../../../../VERSION)

# Package

npx appdmg "appdmg/config.json" "apk-editor-studio_macos_$VERSION.dmg" || exit
