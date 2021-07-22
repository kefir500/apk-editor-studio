rem Prepare

set /p VERSION=<%~dp0\..\..\..\..\VERSION

rem Build

set PATH=%PATH%;%QTDIR%\bin
qmake "%~dp0\..\..\..\.." "DESTDIR=build_apk-editor-studio" || exit /b
nmake || exit /b

rem Package

set OUTPUT=apk-editor-studio_windows_%VERSION%.msi
candle -dDistDir=build_apk-editor-studio -arch x64 -ext WixUIExtension "%~dp0\wix\main.wxs" "%~dp0\wix\ui.wxs" || exit /b
light -out %OUTPUT% -ext WixUIExtension main.wixobj ui.wixobj || exit /b

rem Clean

nmake clean
del *.rc
del .qmake.stash
del Makefile*
del *.wixobj
del *.wixpdb
rmdir debug
rmdir release
rmdir /s /q build_apk-editor-studio
