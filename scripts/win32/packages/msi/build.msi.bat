if /i not "%CI%"=="True" call "%~dp0\..\..\environment.bat"

rem Build

call "%VCVARS%" x86
call "%QTDIR%\bin\qmake" "%~dp0\..\..\..\.." "DESTDIR=\"%~dp0\build"
if %errorlevel% neq 0 exit /b 1
"%MAKE%"
if %errorlevel% neq 0 exit /b 2
"%MAKE%" clean

rem Package

pushd "%~dp0\wix"
if /i not "%CI%"=="True" (
    set OUTPUT=bin\apk-editor-studio_win32_%VERSION%.msi
) else (
    set OUTPUT=bin\apk-editor-studio_win32_dev.msi
)
"%WIXDIR%\candle.exe" -out obj\ -arch x86 -ext WixUIExtension main.wxs ui.wxs
if %errorlevel% neq 0 exit /b 3
"%WIXDIR%\light.exe" -out %OUTPUT% -pdbout bin\apk-editor-studio.wixpdb -ext WixUIExtension obj\main.wixobj obj\ui.wixobj
if %errorlevel% neq 0 exit /b 4
popd

rem Clean

del *.rc
del .qmake.stash
del Makefile*
del "%~dp0\wix\bin\*.wixpdb"
rmdir debug
rmdir release
rmdir /s /q "%~dp0\build"
rmdir /s /q "%~dp0\wix\obj"
