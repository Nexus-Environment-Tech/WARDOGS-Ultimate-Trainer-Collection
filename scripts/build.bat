@echo off
REM MK12Customizer Build Script
REM Requires Visual Studio 2019 (v142) or VS2022 with v142 [citation:4]

echo Building MK12Customizer...
echo.

call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"

msbuild MK12Customizer.sln /p:Configuration=Release /p:Platform=x64

if %ERRORLEVEL% NEQ 0 (
    echo Build failed!
    pause
    exit /b %ERRORLEVEL%
)

echo.
echo Build successful!
echo Output: bin\Release\MK12Customizer.ehp
pause
