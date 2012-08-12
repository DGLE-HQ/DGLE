@echo off

:: Add removeJunk call for every new project!

call :removeVSJunk "" "engine"
call :removeVSJunk "tests" "tests\dev"

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