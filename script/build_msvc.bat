cd %~dp0..

set BUILD_TYPE=%1
set BUILD_BITS=%2
set BUILD_COMPILE=%3

rem script params: msvc.bat [BUILD_TYPE] [BUILD_COMPILE] [BUILD_BITS]
rem default value: BUILD_TYPE=Debug BUILD_COMPILE=VS2015 BUILD_BITS=x86

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

if exist "build\dependency" (goto _tag_build_trace_src)

:_tag_build_trace_3rd
rem ===== build trace <3rd> =====

mkdir build\build_3rd\zeromq
cd build\build_3rd\zeromq

mkdir zeromq
cd zeromq
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ^
-DBUILD_SHARED=ON ^
-DBUILD_STATIC=OFF ^
-DBUILD_TESTS=OFF ^
-DZMQ_BUILD_TESTS=OFF ^
-DCMAKE_INSTALL_PREFIX=%~dp0..\build\dependency\zeromq\zeromq ^
-DWITH_LIBSODIUM=OFF ^
../../../../3rd/zeromq
cmake --build . --config %BUILD_TYPE% -j 8
cmake --install . --config %BUILD_TYPE%
cd ..

mkdir cppzmq
cd cppzmq
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ^
-DCPPZMQ_BUILD_TESTS=OFF ^
-DCMAKE_INSTALL_PREFIX=%~dp0..\build\dependency\zeromq\cppzmq ^
-DZeroMQ_DIR=%~dp0..\build\dependency\zeromq\zeromq\CMake ^
../../../../3rd/cppzmq
cmake --build . --config %BUILD_TYPE% -j 8
cmake --install . --config %BUILD_TYPE%
cd ..

cd ..\..\..

mkdir build\dependency\cereal
cd build\dependency\cereal
mkdir include
xcopy %~dp0..\3rd\cereal\include include /E /Y
cd ..\..\..

:_tag_build_trace_src
rem ===== build trace <src> =====

mkdir build\build_src
cd build\build_src
cmake -G "%BUILD_COMPILE%" %BUILD_ARCH_ARG% ../../src
cmake --build . --config %BUILD_TYPE% -j 8
cd ..\..

:_tag_archive
rem ===== archive =====

mkdir build\archive
cd build\archive

rem [### Copy dependency header files ###]
mkdir include\3rd\zeromq
xcopy ..\dependency\zeromq\zeromq\include include\3rd\zeromq /E /Y
mkdir include\3rd\cppzmq
xcopy ..\dependency\zeromq\cppzmq\include include\3rd\cppzmq /E /Y
mkdir include\3rd\cereal
xcopy ..\dependency\cereal\include include\3rd\cereal /E /Y

rem [### Copy TiMultiServerSupport header files ###]
mkdir include\core
xcopy ..\..\include include\core /E /Y

rem [### Copy ZeroMQ lib and dll files ###]
mkdir libraries
copy ..\dependency\zeromq\zeromq\lib\libzmq.lib libraries /Y
copy ..\dependency\zeromq\zeromq\bin\libzmq.dll libraries /Y

rem [### Copy USER CMakeLists file ###]
copy ..\..\script\TMSS.CMakeLists.txt .\CMakeLists.txt /Y

rem [### Copy LICENSE files ###]
mkdir licenses
copy ..\..\3rd\zeromq\COPYING.LESSER licenses\zeromq.txt /Y
copy ..\..\3rd\cppzmq\LICENSE licenses\cppzmq.txt /Y
copy ..\..\3rd\cereal\LICENSE licenses\cereal.txt /Y
copy ..\..\License.txt .\License.txt /Y

cd ..\..
