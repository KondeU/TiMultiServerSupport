cd %~dp0

set BUILD_BITS=x64
set BUILD_TYPE=%1
set BUILD_COMPILE=%2

echo build parameters: Type(Debug/Release) Compiler(VS2015/VS2017/VS2019)
echo default value: BUILD_TYPE=Debug BUILD_COMPILE=VS2015 BUILD_BITS=x64
echo BUILD_BITS = x64
echo BUILD_TYPE = %BUILD_TYPE%
echo BUILD_COMPILE = %BUILD_COMPILE%

if "%BUILD_TYPE%"=="" (set BUILD_TYPE=Debug)
if "%BUILD_COMPILE%"=="" (set BUILD_COMPILE=VS2015)

if "%BUILD_COMPILE%"=="VS2015" (set BUILD_COMPILE=Visual Studio 14 2015)
if "%BUILD_COMPILE%"=="VS2017" (set BUILD_COMPILE=Visual Studio 15 2017)
if "%BUILD_COMPILE%"=="VS2019" (set BUILD_COMPILE=Visual Studio 16 2019)

rem only VS2019 uses "BUILD_ARCH_ARG"
set BUILD_ARCH_ARG=
if "%BUILD_COMPILE%"=="Visual Studio 16 2019" (
if "%BUILD_BITS%"=="x64" (set BUILD_ARCH_ARG=-A x64) ^
else (set BUILD_ARCH_ARG=-A Win32)
) else (
if "%BUILD_BITS%"=="x64" (set BUILD_COMPILE=%BUILD_COMPILE% Win64)
)

mkdir build\solution
cd build\solution
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ../..
cmake --build . --config %BUILD_TYPE% -j 8
cd ..\..

mkdir build\archive
cd build\archive
copy ..\solution\%BUILD_TYPE%\*.exe . /Y
copy ..\solution\%BUILD_TYPE%\*.pdb . /Y
rem deploy qt program
rem [vcredist(we do not set VCINSTALLDIR)]
rem Qt5_DIR=[...]\Qt5.14.2\5.14.2\msvc2015_64\lib\cmake\Qt5
"%Qt5_DIR%\..\..\..\bin\windeployqt.exe" online_table.exe
cd ..\..
