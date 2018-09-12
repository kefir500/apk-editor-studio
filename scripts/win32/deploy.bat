call environment.bat

rem Deploy Qt Libraries

xcopy /y "%QTDIR%\bin\Qt5Core.dll" ..\..\bin\win32\
xcopy /y "%QTDIR%\bin\Qt5Gui.dll" ..\..\bin\win32\
xcopy /y "%QTDIR%\bin\Qt5Network.dll" ..\..\bin\win32\
xcopy /y "%QTDIR%\bin\Qt5Widgets.dll" ..\..\bin\win32\
xcopy /y "%QTDIR%\bin\Qt5Xml.dll" ..\..\bin\win32\
xcopy /y "%QTDIR%\plugins\imageformats\qgif.dll" ..\..\bin\win32\imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qico.dll" ..\..\bin\win32\imageformats\
xcopy /y "%QTDIR%\plugins\imageformats\qjpeg.dll" ..\..\bin\win32\imageformats\
xcopy /y "%QTDIR%\plugins\platforms\qwindows.dll" ..\..\bin\win32\platforms\

rem Deploy Visual C++ Redistributable

xcopy /y "%VCREDIST%\msvcp*.dll" ..\..\bin\win32\
xcopy /y "%VCREDIST%\msvcr*.dll" ..\..\bin\win32\
xcopy /y "%VCREDIST%\vccorlib*.dll" ..\..\bin\win32\
xcopy /y "%VCREDIST%\vcruntime*dll" ..\..\bin\win32\

rem Deploy OpenSSL

xcopy /y "%OPENSSL%\ssleay32.dll" ..\..\bin\win32\
xcopy /y "%OPENSSL%\libeay32.dll" ..\..\bin\win32\
