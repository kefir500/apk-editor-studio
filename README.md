# APK Editor Studio

[![Build Status (Travis CI)](https://travis-ci.org/kefir500/apk-editor-studio.svg?branch=master)](https://travis-ci.org/kefir500/apk-editor-studio/builds)
[![Build Status (AppVeyor)](https://ci.appveyor.com/api/projects/status/github/kefir500/apk-editor-studio?svg=true)](https://ci.appveyor.com/project/kefir500/apk-editor-studio)
[![Latest Release](https://img.shields.io/github/release/kefir500/apk-editor-studio.svg?maxAge=86400)](https://github.com/kefir500/apk-editor-studio/releases/latest)
[![License](https://img.shields.io/github/license/kefir500/apk-editor-studio?color=blue)](https://raw.githubusercontent.com/kefir500/apk-editor-studio/master/LICENSE)
[![Donate via PayPal](https://img.shields.io/badge/donate-PayPal-orange.svg?logo=paypal)](https://paypal.me/kefir500)
[![Become a Patron](https://img.shields.io/badge/donate-Patreon-orange.svg?logo=patreon)](https://www.patreon.com/kefir500)

**APK Editor Studio** is a powerful yet easy to use APK reverse-engineering tool written in C++/Qt.

- [Website](https://qwertycube.com/apk-editor-studio/)
- [Donate](https://qwertycube.com/donate/)
- [Issues](https://github.com/kefir500/apk-editor-studio/issues)

## Downloads

- [Latest stable release](https://qwertycube.com/apk-editor-studio/download/)
- [Latest unstable build](https://github.com/kefir500/apk-editor-studio/releases/tag/development)
- [Other releases](https://github.com/kefir500/apk-editor-studio/releases)

## Features

- Resource Manager
- Icon Editor
- Title Editor
- Image Editor
- Code Editor
- Manifest Editor
- Permission Editor
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

Also, on **Linux** you may want an optional support for `libsecret` password storage backend.
In order to enable it, you will need to install the corresponding library:
- `libsecret-1-dev` (Debian-based distributions)
- `libsecret-devel` (RPM-based distributions)

### Building

- `qmake`
- `make`

Note that the build tools may vary on different platforms (e.g., `make`/`nmake`/`jom`).
You can also use Qt Creator IDE to compile APK Editor Studio
without manually specifying the paths and running the build tools.

Optionally, run `make install` to install APK Editor Studio to `/usr`.  
Pass the `PREFIX` variable to `qmake` in order to define a different installation directory.

### Packaging

If you want to create installation/executable APK Editor Studio packages,
the corresponding scripts are located at `scripts/**/packages` (where `**` is a platform name).
Every platform directory contains an `environment` script file which sets the paths to the
required tools. Feel free to edit these variables to correspond the paths on your system.

## Translation

To improve the crowdsourcing experience for non-developers, this project is translated
on [Transifex](https://www.transifex.com/qwertycube/apk-editor-studio/).
Please follow the [translation instructions](https://github.com/kefir500/apk-editor-studio/wiki/Translation-Guide)
and avoid direct editing of the language files in this repo.


## Third-Party Components

- [Apktool](https://github.com/iBotPeaches/Apktool)
- [QtKeychain](https://github.com/frankosterfeld/qtkeychain)
- [Android SDK Tools](https://android.googlesource.com)

## Notice

The repacked APK files should not violate the original licenses.
