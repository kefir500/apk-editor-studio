rem Prepare

set /p VERSION=<%~dp0\..\..\..\..\VERSION

rem Build

cmake "%~dp0\..\..\..\.." ^
    -A Win32 ^
    -B build ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY=. ^
    -DCMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE=. ^
    -DCMAKE_PREFIX_PATH="%QTDIR%" ^
    -DOPENSSL_ROOT_DIR=%OPENSSL%
cmake --build build --config Release || exit /b

rem Package

set OUTPUT=apk-editor-studio_windows_%VERSION%.msi
candle -dDistDir=build -arch x86 -ext WixUIExtension "%~dp0\wix\main.wxs" "%~dp0\wix\ui.wxs" || exit /b
light -out %OUTPUT% -ext WixUIExtension main.wixobj ui.wixobj || exit /b
