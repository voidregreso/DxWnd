windres TESTMCI.rc -O coff -o TESTMCI.res
gcc TESTMCI.c TESTMCI.res -Wl,--subsystem,windows -lwinmm -o ../TESTMCI.exe
pause

REM The -Wl,--subsystem,windows linker switch ensures that the application is built as a Windows GUI application, and not a console application.