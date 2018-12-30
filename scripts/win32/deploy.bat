call "%~dp0\environment.bat"

rem Deploy Qt Libraries

xcopy /y "%QTDIR%\bin\Qt5Core.dll" .
xcopy /y "%QTDIR%\bin\Qt5Gui.dll" .
xcopy /y "%QTDIR%\bin\Qt5Network.dll" .
xcopy /y "%QTDIR%\bin\Qt5Svg.dll" .
xcopy /y "%QTDIR%\bin\Qt5Widgets.dll" .
xcopy /y "%QTDIR%\bin\Qt5Xml.dll" .
xcopy /y "%QTDIR%\plugins\imageformats\qgif.dll" imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qicns.dll" imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qico.dll" imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qjpeg.dll"\imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qsvg.dll" imageformats\
xcopy /y "%QTDIR%\plugins\platforms\qwindows.dll" platforms\
xcopy /y "%QTDIR%\plugins\styles\qwindowsvistastyle.dll" styles\

rem Deploy Visual C++ Redistributable

xcopy /y "%VCREDIST%\msvcp*.dll" .
xcopy /y "%VCREDIST%\msvcr*.dll" .
xcopy /y "%VCREDIST%\vccorlib*.dll" .
xcopy /y "%VCREDIST%\vcruntime*dll" .

rem Deploy OpenSSL

xcopy /y "%OPENSSL%\ssleay32.dll" .
xcopy /y "%OPENSSL%\libeay32.dll" .
