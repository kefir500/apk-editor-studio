if /i not "%CI%"=="True" call "%~dp0\..\..\environment.bat"

rem Prepare

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
"%QTDIR%\bin\qmake" "%~dp0\..\..\..\.." "DESTDIR=\"%FOLDER%\"" DEFINES+=PORTABLE
if %errorlevel% neq 0 exit /b 1
"%MAKE%"
if %errorlevel% neq 0 exit /b 2
"%MAKE%" clean

rem Deploy Qt Libraries

xcopy /y "%QTDIR%\bin\Qt5Core.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\bin\Qt5Gui.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\bin\Qt5Network.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\bin\Qt5Svg.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\bin\Qt5Widgets.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\bin\Qt5Xml.dll" "%FOLDER%\"
xcopy /y "%QTDIR%\plugins\imageformats\qgif.dll" "%FOLDER%\imageformats\"
xcopy /y "%QTDIR%\plugins\imageformats\qicns.dll" "%FOLDER%\imageformats\"
xcopy /y "%QTDIR%\plugins\imageformats\qico.dll" "%FOLDER%\imageformats\"
xcopy /y "%QTDIR%\plugins\imageformats\qjpeg.dll" "%FOLDER%\imageformats\"
xcopy /y "%QTDIR%\plugins\imageformats\qsvg.dll" "%FOLDER%\imageformats\"
xcopy /y "%QTDIR%\plugins\platforms\qwindows.dll" "%FOLDER%\platforms\"
xcopy /y "%QTDIR%\plugins\styles\qwindowsvistastyle.dll" "%FOLDER%\styles\"

rem Deploy Visual C++ Redistributable

xcopy /y "%VCREDIST_DLL%\msvcp*.dll" "%FOLDER%\"
xcopy /y "%VCREDIST_DLL%\msvcr*.dll" "%FOLDER%\"
xcopy /y "%VCREDIST_DLL%\vcruntime*dll" "%FOLDER%\"

rem Deploy OpenSSL

xcopy /y "%OPENSSL%\libcrypto-1_1.dll" "%FOLDER%\"
xcopy /y "%OPENSSL%\libssl-1_1.dll" "%FOLDER%\"

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
