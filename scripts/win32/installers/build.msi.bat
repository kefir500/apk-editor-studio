call ..\environment.bat

cd msi\wix
rmdir /s /q bin
rmdir /s /q obj

"%WIXDIR%\candle.exe" -out obj\ -arch x86 -ext WixUIExtension main.wxs ui.wxs
"%WIXDIR%\light.exe" -out bin\apk-editor-studio_win32_%VERSION%.msi -pdbout bin\apk-editor-studio.wixpdb -ext WixUIExtension obj\main.wixobj obj\ui.wixobj

copy bin\apk-editor-studio_win32_%VERSION%.msi ..\..
cd ..\..
