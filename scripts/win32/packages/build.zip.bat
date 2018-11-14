call ..\environment.bat

if /i not "%VERSION%"=="dev" (
    set FOLDER="APK Editor Studio v%VERSION%"
) else (
    set FOLDER="APK Editor Studio - Dev"
)

if exist apk-editor-studio_win32_%VERSION%.zip del apk-editor-studio_win32_%VERSION%.zip
if exist %FOLDER% rmdir /s /q %FOLDER%

xcopy /s ..\..\..\bin\win32 %FOLDER%\

"%SEVENZIP%" a -tzip -mx=9 apk-editor-studio_win32_%VERSION%.zip %FOLDER%\* -xr!*.pdb

rmdir /s /q %FOLDER%
