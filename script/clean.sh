#!/bin/bash

set -e
# Please run this script by ../build.sh
# Otherwise the path will be incorrect!
TOP_PATH=$(cd "$(dirname "$0")"; pwd)
echo ==== clean: delete the folder: ${TOP_PATH}/build ====

rm -rf ${TOP_PATH}/build
