# APK Editor Studio

[![Build Status (Travis CI)](https://travis-ci.org/kefir500/apk-editor-studio.svg?branch=master)](https://travis-ci.org/kefir500/apk-editor-studio/builds)
[![Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/github/kefir500/apk-editor-studio?svg=true)](https://ci.appveyor.com/project/kefir500/apk-editor-studio)
[![Latest Release](https://img.shields.io/github/release/kefir500/apk-editor-studio.svg?maxAge=86400)](https://github.com/kefir500/apk-editor-studio/releases/latest)
[![License](https://img.shields.io/badge/license-GPLv3-blue.svg?maxAge=2592000)](https://raw.githubusercontent.com/kefir500/apk-editor-studio/master/LICENSE)
[![PayPal Donate](https://img.shields.io/badge/donate-PayPal-orange.svg?logo=paypal)](https://qwertycube.com/apk-editor-studio/donate/)

**APK Editor Studio** is a powerful yet easy to use APK reverse-engineering tool written in C++/Qt.

## Links

- [Website](https://qwertycube.com/apk-editor-studio/)
- [Download](https://qwertycube.com/apk-editor-studio/)
- [Issues](https://github.com/kefir500/apk-editor-studio/issues)

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

### Setting Up

**APK Editor Studio** depends on multiple third-party binary tools which are not included in this repository.
In order to successfully utilize this software, the following tools should be present on your system:

- `apktool` to (un)pack APK
- `apksigner` to sign APK
- `zipalign` to optimize APK
- `adb` to install APK and manage Android devices

You can either get them manually (e.g., using `apt-get` in Debian)
or **run the `scripts/download.py`** Python 3 script which will automatically download the needed tools.

Also, on **Linux** you may want to optionally install `libsecret` development library to use it as a password storage,
otherwise GNOME Keyring / KWallet will be used to store your KeyStore passwords.  
Package aliases:
- `libsecret-1-dev` (Debian-based distributions)
- `libsecret-devel` (RPM-based distributions)

### Building

- `qmake`
- `make`

Note that build tools may vary on different platforms (e.g., `make`/`nmake`).
You can also use Qt Creator IDE to compile APK Editor Studio
without manually specifying the paths and running the build tools.

Optionally, run `make install` to install APK Editor Studio to `/usr`.  
Pass the `PREFIX` variable to `qmake` in order to define a different installation directory.

### Packaging

If you want to create installation/executable APK Editor Studio packages,
the corresponding scripts are located at `scripts/**/packages` (where `**` is a platform name).

## Third-Party Components

- [Apktool](https://github.com/iBotPeaches/Apktool)
- [QtKeychain](https://github.com/frankosterfeld/qtkeychain)
- [Android Open Source Project](https://github.com/aosp-mirror) tools

## Notice

- You may not use **APK Editor Studio** for any illegal purposes.
- The repacked APK files should not violate the original licenses.
