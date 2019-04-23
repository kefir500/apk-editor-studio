rem The following lines set the paths to the tools required for build.
rem Feel free to change them to correspond the paths on your system.

if not defined QTDIR        set QTDIR=C:\Qt\Qt5.12.2\5.12.2\msvc2017
if not defined OPENSSL      set OPENSSL=C:\OpenSSL\msvc2017
if not defined VCVARS       set VCVARS=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Auxiliary\Build\vcvarsall.bat
if not defined VCREDIST_DLL set VCREDIST_DLL=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012\x86\Microsoft.VC141.CRT
if not defined VCREDIST_MSM set VCREDIST_MSM=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community\VC\Redist\MSVC\14.16.27012\MergeModules\Microsoft_VC141_CRT_x86.msm
if not defined WIXDIR       set WIXDIR=C:\Program Files (x86)\WiX Toolset v3.11\bin
if not defined SEVENZIP     set SEVENZIP=C:\Program Files\7-Zip\7z.exe

rem The following line sets the application version number variable. Don't change it.

if not defined VERSION for /f %%i in ('type %0\..\..\..\VERSION') do (set VERSION=%%i)
