echo off
echo "+------------------------------------------------+
echo "|                DxWnd kit builder               |
echo "| manual steps:                                  |
echo "| 1) check dxwnd version string                  |
echo "| 2) rebuild all                                 |
echo "| 3) set admin caps to DxWnd.exe                 |
echo "| 4) back up important dxwnd.ini file            |
echo "| 5) remove clean.bat from source archive        |
echo "+------------------------------------------------+

echo "Ctrl-C to abort the builder"
echo "Clearing projects"
pause

echo on
call :cleanfolder dll
call :cleanfolder host
call :cleanfolder proxy
call :cleanfolder ddraw.tracer
call :cleanfolder filter
call :cleanfolder dxwplay
call :cleanfolder ExportFinder
call :cleanfolder hqx
call :cleanfolder mcitool
call :cleanfolder blacker
call :cleanfolder cdplayer

echo "Clearing working folders"
pause

rmdir /S /Q Release
rmdir /S /Q Debug

echo "Clearing dynamic data"
pause

del /Q build\dxwnd.ini
del /Q build\dxwnd.log
del /Q build\dxwnd.gui.log
del /Q build\dxwnd.bak
del /Q build\dxwplay.log
del /Q build\dxwnd.reg
del /Q build\pedump.txt

echo "Clearing virus false positive files"
pause

del /Q build\proxy\winmm.dll

echo "Clearing pictures"
pause

rmdir /S /Q build\screenshot.out
rmdir /S /Q build\surface.out

echo "done!"
pause
exit /B 0

:cleanfolder
del /AH /Q %*\*.suo
del /Q %*\*.user
del /Q %*\*.ncb
del /Q %*\*.aps
del /S /Q %*\Release
del /S /Q %*\Debug
rmdir %*\Release
rmdir %*\Debug
exit /B 0
