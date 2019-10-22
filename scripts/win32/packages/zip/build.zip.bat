if /i not "%CI%"=="True" call "%~dp0\..\..\environment.bat"

rem Prepare

set ROOT=%~dp0\..\..\..\..

if /i not "%CI%"=="True" (
    set FOLDER=APK Editor Studio v%VERSION%
    set ARCHIVE=%~dp0\apk-editor-studio_win32_%VERSION%.zip
) else (
    set FOLDER=APK Editor Studio - Developer Build
    set ARCHIVE=%~dp0\apk-editor-studio_win32_dev.zip
)

if exist "%ARCHIVE%" del "%ARCHIVE%"
if exist "%FOLDER%" rmdir /s /q "%FOLDER%"

rem Build

call "%VCVARS%" x86
"%QTDIR%\bin\qmake" "%ROOT%" "DESTDIR=\"%FOLDER%\"" DEFINES+=PORTABLE
if %errorlevel% neq 0 exit /b 1
"%MAKE%"
if %errorlevel% neq 0 exit /b 2
"%MAKE%" clean

rem Deploy

cd "%FOLDER%"
call "%~dp0\..\..\deploy.bat"
cd ..

rem Package

"%SEVENZIP%" a -tzip -mx=9 "%ARCHIVE%" "%FOLDER%\*" -xr!*.pdb
if %errorlevel% neq 0 exit /b 3

rem Clean

del *.rc
del .qmake.stash
del Makefile*
rmdir debug
rmdir release
rmdir /s /q "%FOLDER%"
