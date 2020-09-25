rem The following lines set the paths to the tools required for build.
rem Feel free to change them to correspond the paths on your system.

set QTDIR=C:\Qt\5.15.0\msvc2019
set OPENSSL=C:\Qt\Tools\OpenSSL\Win_x86\bin
set WIXDIR=C:\Program Files (x86)\WiX Toolset v3.11\bin
SET PATH=%PATH%;C:\Program Files\7-Zip
call "C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat" x86

rem The following line sets the application version number variable. Don't change it.

set /p VERSION=<%0\..\..\..\VERSION
