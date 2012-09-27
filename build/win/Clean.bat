@echo off

:: PATHS
set BIN_WIN_32_PATH=.\..\..\bin\win
set BIN_WIN_64_PATH=.\..\..\bin\win\x64
set SRC_PATH=.\..\..\src


:: Add removeJunk call for every new project!

call :removeVSJunk "" "engine"
call :removeVSJunk "tests" "tests\dev"

:: Wipe up Tools
call :delete_junk %BIN_WIN_32_PATH%\tools
call :delete_junk %BIN_WIN_64_PATH%\tools
call :wipe_project %SRC_PATH%\tools\gui_widgets
call :wipe_project %SRC_PATH%\tools\template
call :wipe_solution .\..\common\tools


:: Function fil clear all temporary Visual Studio files.
:removeVSJunk

set arg1=%1
set arg2=%2

del ..\..\bin\win\%arg1%\*.pdb
del ..\..\bin\win\%arg1%\*.exp
del ..\..\bin\win\%arg1%\*.lib
del ..\..\bin\win\%arg1%\*.ilk
del ..\..\bin\win\%arg1%\log.txt
del ..\..\bin\win\x64\%arg1%\*.pdb
del ..\..\bin\win\x64\%arg1%\*.exp
del ..\..\bin\win\x64\%arg1%\*.lib
del ..\..\bin\win\x64\%arg1%\*.ilk
del ..\..\bin\win\x64\%arg1%\log.txt

del %arg2%\*.sdf
del %arg2%\*.suo
del %arg2%\log.txt
rd /s /q %arg2%\ipch
rd /s /q %arg2%\Debug
rd /s /q %arg2%\Release
rd /s /q %arg2%\x64\Debug
rd /s /q %arg2%\x64\Release
rd %arg2%\x64

goto :eof



:: Wipe up solution tools
:wipe_solution
call :delete_junk %1
goto:eof

:: Wipe up project in folder(%1)
:wipe_project
if "%1"=="" goto:eof
pushd "%1"
rd obj /q /s
popd
goto:eof

:: Delete temporary files in folder(%1)
:delete_junk
if "%1"=="" goto:eof
pushd "%1"
del *.userprefs /q
del *.suo /q /a:h
del *.pdb /q
del *.lib /q
del *.exp /q
del *.lib /q
del log.txt /q
del *.ilk /q
del *.map /q
del *.tds /q
del *.dcu /q
del *.vshost.exe /q
del *.vshost.exe.manifest /q
popd
goto:eof