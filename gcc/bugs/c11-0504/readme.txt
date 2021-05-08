Steps to reproduce:

git clone --recurse https://github.com/google/googletest
cd googeltest
gcc11env.cmd
set CC=C:\usr\local11\bin\gcc.exe
set CXX=C:\usr\local11\bin\g++.exe
mkdir _b
cd _b
cmake ..
cmake --build .


Rem:
Here was what I ran that failed:
cd "C:\git/wwiv/_b11/deps/googletest/googletest" && "/usr/local11/bin/g++.exe"   @CMakeFiles/gtest.dir/includes_CXX.rsp -O3 -DNDEBUG   -Wall -Wshadow -Werror -Wno-error=dangling-else -DGTEST_HAS_PTHREAD=0 -fexceptions -Wextra -Wno-unused-parameter -Wno-missing-field-initializers -std=c++17 -o CMakeFiles/gtest.dir/src/gtest-all.cc.o -c C:/git/wwiv/deps/googletest/googletest/src/gtest-all.cc

