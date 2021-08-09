#!/bin/bash

set -e
# Please run this script by ../build.sh
# Otherwise the path will be incorrect!
TOP_PATH=$(cd "$(dirname "$0")"; pwd)

# See ../build.sh
BUILD_TYPE="$1"
BUILD_BITS="$2"

if test -z ${BUILD_TYPE} ; then
BUILD_TYPE="Debug"
fi

if test -z ${BUILD_BITS} ; then
BUILD_BITS="Undefined"
fi

BUILD_JOBS=$(($(grep -c ^processor /proc/cpuinfo) - 1))

# The building in linux will use the default compile toolchain in the system.
echo ==== build: building TiRPC now. ====

echo Build type: ${BUILD_TYPE}
echo Build bits: ${BUILD_BITS}
if [ ${BUILD_BITS} = "x64" ] ; then
export CFLAGS="-m64 ${CFLAGS}"
export CXXFLAGS="-m64 ${CXXFLAGS}"
elif [ ${BUILD_BITS} = "x86" ] ; then
export CFLAGS="-m32 ${CFLAGS}"
export CXXFLAGS="-m32 ${CXXFLAGS}"
else
echo Undefined or unknown build bit: ${BUILD_BITS}, use the default.
fi

export MAKEFLAGS="${MAKEFLAGS} -j${BUILD_JOBS}"
echo Build jobs count: ${BUILD_JOBS}

echo Build Config Infos of Env:
echo - CFLAGS:    ${CFLAGS}
echo - CXXFLAGS:  ${CXXFLAGS}
echo - MAKEFLAGS: ${MAKEFLAGS}

#==========================================================================================

if [ ! -d "${TOP_PATH}/build/dependency" ] ; then

echo Building dependent open source code...

cd "${TOP_PATH}"

mkdir -p build/build_3rd/zeromq
cd build/build_3rd/zeromq

echo - Build open source: zeromq
mkdir -p zeromq
cd zeromq
cmake \
-DBUILD_SHARED=ON \
-DBUILD_STATIC=OFF \
-DBUILD_TESTS=OFF \
-DZMQ_BUILD_TESTS=OFF \
-DCMAKE_INSTALL_PREFIX="${TOP_PATH}/build/dependency/zeromq/zeromq" \
-DWITH_LIBSODIUM=OFF \
../../../../3rd/zeromq
cmake --build . --config ${BUILD_TYPE}
make install # `cmake --install .` does not work.
cd ..

echo - Build open source: cppzmq
mkdir -p cppzmq
cd cppzmq
cmake \
-DCPPZMQ_BUILD_TESTS=OFF \
-DCMAKE_INSTALL_PREFIX="${TOP_PATH}/build/dependency/zeromq/cppzmq" \
-DZeroMQ_DIR="${TOP_PATH}/build/dependency/zeromq/zeromq/lib/cmake/ZeroMQ" \
../../../../3rd/cppzmq
cmake --build . --config ${BUILD_TYPE}
make install # `cmake --install .` does not work.
cd ..

cd ../../..

echo - Build open source: cereal
mkdir -p build/dependency/cereal
cd build/dependency/cereal
# Note that the `cp` here is that copy the entire folder, not all the files
# in the folder, so the new location will has the `include` directory.
cp -ruv "${TOP_PATH}/3rd/cereal/include" .
cd ../../..

fi

#==========================================================================================

echo Building demo and test code...

echo - Build demo and test: TestStandardRpc, TestBroadcastRpc
mkdir -p build/build_src
cd build/build_src
cmake ../../src
cmake --build . --config ${BUILD_TYPE}
cd ../..

#================================================================================

echo Archiving...

mkdir -p build/archive
cd build/archive

echo - Copy dependent open source header files
mkdir -p include/3rd/zeromq
cp -ruv ../dependency/zeromq/zeromq/include/* include/3rd/zeromq
mkdir -p include/3rd/cppzmq
cp -ruv ../dependency/zeromq/cppzmq/include/* include/3rd/cppzmq
mkdir -p include/3rd/cereal
cp -ruv ../dependency/cereal/include/* include/3rd/cereal

echo - Copy TiRPC header files
mkdir -p include/core
cp -ruv ../../include include/core

echo - Copy ZeroMQ lib and dll files
mkdir -p libraries
cp -u ../dependency/zeromq/zeromq/lib/libzmq.so* libraries

echo - Copy USER CMakeLists file
cp -u ../../script/TiRPC.cmake ./TiRPC.cmake

echo - Copy LICENSE files
mkdir -p licenses
cp -u ../../3rd/zeromq/COPYING.LESSER licenses/zeromq.txt
cp -u ../../3rd/cppzmq/LICENSE licenses/cppzmq.txt
cp -u ../../3rd/cereal/LICENSE licenses/cereal.txt
cp -u ../../LICENSE.txt ./License.txt

cd ../..

#================================================================================

echo ====================
echo Build successfully!!
echo ====================
