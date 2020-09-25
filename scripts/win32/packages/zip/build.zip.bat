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

call "%QTDIR%\bin\qtenv2.bat"
qmake "%~dp0\..\..\..\.." "DESTDIR=\"%FOLDER%\"" DEFINES+=PORTABLE || exit /b
nmake || exit /b
nmake clean

rem Deploy Qt Libraries

xcopy /y "%QTDIR%\bin\Qt5Core.dll" "%FOLDER%\"    || exit /b
xcopy /y "%QTDIR%\bin\Qt5Gui.dll" "%FOLDER%\"     || exit /b
xcopy /y "%QTDIR%\bin\Qt5Network.dll" "%FOLDER%\" || exit /b
xcopy /y "%QTDIR%\bin\Qt5Svg.dll" "%FOLDER%\"     || exit /b
xcopy /y "%QTDIR%\bin\Qt5Widgets.dll" "%FOLDER%\" || exit /b
xcopy /y "%QTDIR%\bin\Qt5Xml.dll" "%FOLDER%\"     || exit /b
xcopy /y "%QTDIR%\plugins\imageformats\qgif.dll" "%FOLDER%\imageformats\"   || exit /b
xcopy /y "%QTDIR%\plugins\imageformats\qicns.dll" "%FOLDER%\imageformats\"  || exit /b
xcopy /y "%QTDIR%\plugins\imageformats\qico.dll" "%FOLDER%\imageformats\"   || exit /b
xcopy /y "%QTDIR%\plugins\imageformats\qjpeg.dll" "%FOLDER%\imageformats\"  || exit /b
xcopy /y "%QTDIR%\plugins\imageformats\qsvg.dll" "%FOLDER%\imageformats\"   || exit /b
xcopy /y "%QTDIR%\plugins\platforms\qwindows.dll" "%FOLDER%\platforms\"     || exit /b
xcopy /y "%QTDIR%\plugins\styles\qwindowsvistastyle.dll" "%FOLDER%\styles\" || exit /b

rem Deploy OpenSSL

xcopy /y "%OPENSSL%\libcrypto-1_1.dll" "%FOLDER%\" || exit /b
xcopy /y "%OPENSSL%\libssl-1_1.dll" "%FOLDER%\"    || exit /b

rem Package

7z a -tzip -mx=9 "%ARCHIVE%" "%FOLDER%\*" -xr!*.pdb || exit /b

rem Clean

del *.rc
del .qmake.stash
del Makefile*
rmdir debug
rmdir release
rmdir /s /q "%FOLDER%"
