rem Prepare

set /p VERSION=<%~dp0\..\..\..\..\VERSION
set FOLDER=APK Editor Studio v%VERSION%

rem Build

cmake "%~dp0\..\..\..\.." ^
    -A Win32 ^
    -B build ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_PREFIX_PATH="%QTDIR%" ^
    -DOPENSSL_ROOT_DIR="%OPENSSL%" ^
    -DPORTABLE=ON
cmake --build build --config Release || exit /b
cmake --install build --prefix "%FOLDER%"
del "%FOLDER%\imageformats\qtga.dll"
del "%FOLDER%\imageformats\qtiff.dll"
del "%FOLDER%\imageformats\qwbmp.dll"
rmdir /s /q "%FOLDER%\bearer"
rmdir /s /q "%FOLDER%\iconengines"

rem Package

set ARCHIVE=apk-editor-studio_windows_%VERSION%.zip
7z a -tzip -mx=9 "%ARCHIVE%" "%FOLDER%" || exit /b
