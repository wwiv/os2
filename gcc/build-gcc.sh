#!/usr/bin/ash
set -e
source ./buildenv.sh

make -j4 2>&1 | tee build.log
make install
