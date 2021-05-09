set LIB=C:\msvc\lib
rem c:\msvc\bin\cl -I C:\msvc\include -L C:\msvc\lib %1 %2 %3 %4 %5 %6 dospipe.cpp
rem C:\msvc\bin\link @DOSPIPE.CRF
C:\msvc\bin\nmake /f dospipe.mak %1 %2 %3 %4 %5