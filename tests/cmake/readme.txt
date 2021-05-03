OS/2 GCC 11.1 CMake error.

To reproduce:

Check out the repro
make sure cmake is installed
yum -y install cmake


Here's the commands to run:

mkdir _b
cd _b
..\cc11.cmd

Note that this works if instead of cc11.cmd, just run "cmake .." (which uses the existing
gcc 9.2 compiler)

Error:
====================================================================
Determining if the C compiler works failed with the following output:
Change Dir: C:/git/os2/tests/cmake/_b/CMakeFiles/CMakeTmp

Run Build Command(s):C:/USR/BIN/make.exe cmTC_f4ff2/fast && C:/USR/BIN/make.exe -f CMakeFiles/cmTC_f4ff2.dir/build.make CMakeFiles/cmTC_f4ff2.dir/build
make.exe[1]: Entering directory `C:/git/os2/tests/cmake/_b/CMakeFiles/CMakeTmp'
Building C object CMakeFiles/cmTC_f4ff2.dir/testCCompiler.c.o
C:/usr/local11/bin/gcc.exe    -o CMakeFiles/cmTC_f4ff2.dir/testCCompiler.c.o   -c C:/git/os2/tests/cmake/_b/CMakeFiles/CMakeTmp/testCCompiler.c
Linking C executable cmTC_f4ff2.exe
echo NAME cmTC_f4ff2  > cmTC_f4ff2.def && echo DESCRIPTION \"@#cmake build system:0.0#@##1## 02 May 2021 20:17:53\ \ \ \ \ ARCA506::::0::@@cmTC_f4ff2\" >> cmTC_f4ff2.def && echo  >> cmTC_f4ff2.def && C:/usr/local11/bin/gcc.exe   -Zomf -Zomf -Zlinker DISABLE -Zlinker 1121  @CMakeFiles/cmTC_f4ff2.dir/objects1.rsp -o cmTC_f4ff2.exe   cmTC_f4ff2.def
weakld: cannot open library file 'C:\usr\local11\bin\..\lib\gcc\i686-pc-os2-emx\11.1.0\..\..\..\c_alias_s.a'.
emxomfld: weak prelinker failed. (rc=8)
make.exe[1]: *** [cmTC_f4ff2.exe] Error 1
make.exe[1]: Leaving directory `C:/git/os2/tests/cmake/_b/CMakeFiles/CMakeTmp'
make.exe: *** [cmTC_f4ff2/fast] Error 2



