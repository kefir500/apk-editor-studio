rem The following lines set the paths to the tools required for build.
rem Feel free to change them to correspond the paths on your system.

if not defined QTDIR    set QTDIR=C:\Qt\Qt5.11.1\5.11.1\msvc2015
if not defined OPENSSL  set OPENSSL=C:\Qt\Qt5.11.1\Tools\QtCreator\bin
if not defined VCDIR    set VCDIR=C:\Program Files (x86)\Microsoft Visual Studio 14.0\VC
if not defined VCREDIST set VCREDIST=%VCDIR%\redist\x86\Microsoft.VC140.CRT
if not defined WIXDIR   set WIXDIR=C:\Program Files (x86)\WiX Toolset v3.11\bin
if not defined SEVENZIP set SEVENZIP=C:\Program Files\7-Zip\7z.exe

rem The following line sets the application version number variable. Don't change it.

if not defined VERSION for /f %%i in ('type %0\..\..\..\VERSION') do (set VERSION=%%i)
