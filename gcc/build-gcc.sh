#!/usr/bin/ash
BUILD_DIR=/usr/local11 
mkdir ${BUILD_DIR}

set -e

export PATH="$BUILD_DIR/gcc${PATH:+;$PATH}"
export BEGINLIBPATH="$BUILD_DIR/gcc${BEGINLIBPATH:+;$BEGINLIBPATH}"
export LIBPATHSTRICT=T

echo "PATH:         $PATH"
echo "BEGINLIBPATH: $BEGINLIBPATH"

make -j4 2>&1 | tee build.log
make install
