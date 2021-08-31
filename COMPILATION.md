# Compilation

## Requirements

- **CMake 3.20.0** (or later)
- **C++11** (or later) compiler
- **Qt 5.14** (or later)

On Linux, you will also need the `libsecret-1` development package installed.

## Setting Up

During the run-time, APK Editor Studio uses these third-party tools which are not included in this repository:

- `apktool` to (un)pack APK
- `apksigner` to sign APK
- `zipalign` to optimize APK
- `adb` to install APK and manage Android devices

Running the `scripts/download.py` script will automatically download the needed tools.

## Building

- Run `cmake -B your/build/path -DCMAKE_BUILD_TYPE=Release -DCMAKE_PREFIX_PATH="path/to/qt"`
- Run `cmake --build your/build/path --config Release`

You can also simply use the Qt Creator IDE to build APK Editor Studio.

### Windows notes

To automatically deploy the OpenSSL DLL files on Windows,
pass the `-DOPENSSL_ROOT_DIR="path/to/openssl"` argument along with the first command.
It should point to the OpenSSL installation directory.

## Packaging

If you want to create the APK Editor Studio installation/executable packages,
the corresponding scripts are located at `scripts/**/packages` (where `**` is a platform name).
Refer to the underlying `README.md` file for more information on the specific platform.
