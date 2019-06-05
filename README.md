# APK Editor Studio

[![Build Status (Travis CI)](https://travis-ci.org/kefir500/apk-editor-studio.svg?branch=master)](https://travis-ci.org/kefir500/apk-editor-studio/builds)
[![Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/github/kefir500/apk-editor-studio?svg=true)](https://ci.appveyor.com/project/kefir500/apk-editor-studio)
[![Latest Release](https://img.shields.io/github/release/kefir500/apk-editor-studio.svg?maxAge=86400)](https://github.com/kefir500/apk-editor-studio/releases/latest)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg?maxAge=2592000)](https://raw.githubusercontent.com/kefir500/apk-editor-studio/master/LICENSE)
[![PayPal Donate](https://img.shields.io/badge/donate-PayPal-orange.svg?logo=paypal)](https://qwertycube.com/apk-editor-studio/donate/)

**APK Editor Studio** is a powerful yet easy to use APK reverse-engineering tool written in C++/Qt.

## Features

- Resource Manager
- Icon Editor
- Title Editor
- Image Editor
- Code Editor
- Manifest Editor
- APK Signer
- APK Optimizer
- APK Installer

## Compilation

### Requirements

- Qt 5
- C++11 compiler

### Linux

- `qmake`
- `make`
- `make install`

##### Notes:

- **APK Editor Studio** uses tools from `adb` (`android-tools-adb`) and `zipalign`, however the precompiled binaries are already included in the repository in case of absence of these packages.
- Install `libsecret` development library to use it as a password storage.  
  Otherwise GNOME Keyring / KWallet will be used.  
  Package aliases:
  - `libsecret-1-dev` (Debian-based distributions)
  - `libsecret-devel` (RPM-based distributions)
- By default `make install` installs APK Editor Studio to `/usr`.  
  Pass `PREFIX` variable to `qmake` in order to define a different installation directory.

## Notice

- You may not use **APK Editor Studio** for any illegal purposes.
- The repacked APK files should not violate the original licenses.
