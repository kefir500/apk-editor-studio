setlocal
if /i not "%CI%"=="True" call "%~dp0\..\..\environment.bat"

rem Build

pushd . && call "%QTDIR%\bin\qtenv2.bat" && popd
qmake "%~dp0\..\..\..\.." "DESTDIR=\"%~dp0\build\"" || exit /b
nmake || exit /b

rem Package

pushd "%~dp0\wix"
if /i not "%CI%"=="True" (
    set OUTPUT=bin\apk-editor-studio_win32_%VERSION%.msi
) else (
    set OUTPUT=bin\apk-editor-studio_win32_dev.msi
)
"%WIXDIR%\candle.exe" -out obj\ -arch x86 -ext WixUIExtension main.wxs ui.wxs || exit /b
"%WIXDIR%\light.exe" -out %OUTPUT% -pdbout bin\apk-editor-studio.wixpdb -ext WixUIExtension obj\main.wixobj obj\ui.wixobj || exit /b
popd

rem Clean

nmake clean
del *.rc
del .qmake.stash
del Makefile*
del "%~dp0\wix\bin\*.wixpdb"
rmdir debug
rmdir release
rmdir /s /q "%~dp0\build"
rmdir /s /q "%~dp0\wix\obj"
