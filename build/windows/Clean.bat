@echo off

:: Global paths

set BIN_WIN_32_PATH=.\..\..\bin\windows
set BIN_WIN_64_PATH=.\..\..\bin\windows\x64
set SRC_PATH=.\..\..\src

::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: Don't forget to remove junk for every new project! ::
::::::::::::::::::::::::::::::::::::::::::::::::::::::::

:: Engine

call :removeBinJunkCPP ""

del .\engine\DGLE.APS
call :removeSlnJunkCPP "engine"

call :removeProjJunkCPP "engine"

:: Plugins

call :removeBinJunkCPP "plugins"

call :removeSlnJunkCPP "plugins"

call :removeProjJunkCPP "plugins\template"
call :removeProjJunkCPP "plugins\common"
call :removeProjJunkCPP "plugins\ext"

:: Tests

call :removeBinJunkCPP "tests"

call :removeProjJunkCPP "tests\dev"

call :removeBinJunkCSharp "tests"

call :removeProjJunkCSharp "tests\csharp"

:: Utils

call :removeBinJunkCPP "utils"

call :removeSlnJunkCPP "utils"

call :removeProjJunkCPP "utils\console"

:: Examples

call :removeBinJunkCPP "examples\cpp"
call :removeBinJunkCPPBuilder "examples\cpp_builder"

call :removeSlnJunkCPP "examples\cpp"
call :removeGroupprojJunkCPPBuilder "examples\cpp_builder"

call :removeProjJunkCPP "examples\cpp\3d_in_2d"
call :removeProjJunkCPP "examples\cpp\asteroids_game"
call :removeProjJunkCPP "examples\cpp\basic_2d"
call :removeProjJunkCPP "examples\cpp\core_renderer_and_opengl"
call :removeProjJunkCPP "examples\cpp\hello_world"
call :removeProjJunkCPP "examples\cpp\low_level_2d"
call :removeProjJunkCPP "examples\cpp\low_level_3d"
call :removeProjJunkCPP "examples\cpp\matrix_stack_3d"
call :removeProjJunkCPP "examples\cpp\render_to_texture_2d"

call :removeProjJunkCPPBuilder "examples\cpp_builder\render_to_panel"

:: CSharp assemblies

del .\..\..\include\csharp\*.pdb

call :removeSlnJunkCSharp "..\common\csharp_assembly"
call :removeProjJunkCSharp "engine\csharp_assembly"

:: Tools

call :removeBinJunkCSharp "tools"

call :removeSlnJunkCSharp "..\common\tools"

call :removeProjJunkCSharp "tools\gui"
call :removeProjJunkCSharp "tools\template"
call :removeProjJunkCSharp "tools\font_tool"
call :removeProjJunkCSharp "tools\packer"
call :removeProjJunkCSharp "tools\color_picker"

:: Functions will clear all temporary Embarcadero (Borland) C++ Builder files.

:removeBinJunkCPPBuilder

set arg=%1

del %BIN_WIN_32_PATH%\%arg%\log.txt
del %BIN_WIN_32_PATH%\%arg%\*.map
del %BIN_WIN_32_PATH%\%arg%\*.tds

goto :eof

:removeGroupprojJunkCPPBuilder

set arg=%1

del .\%arg%\*.local

goto :eof

:removeProjJunkCPPBuilder

set arg=%1

del .\..\..\src\%arg%\*.local
rd /s /q .\..\..\src\%arg%\Debug
rd /s /q .\..\..\src\%arg%\Release
rd /s /q .\..\..\src\%arg%\__history

goto :eof

:: Functions will clear all temporary Visual Studio files for C++ project.

:removeBinJunkCPP

set arg=%1

del %BIN_WIN_32_PATH%\%arg%\*.pdb
del %BIN_WIN_32_PATH%\%arg%\*.exp
del %BIN_WIN_32_PATH%\%arg%\*.lib
del %BIN_WIN_32_PATH%\%arg%\*.ilk
del %BIN_WIN_32_PATH%\%arg%\log.txt

del %BIN_WIN_64_PATH%\%arg%\*.pdb
del %BIN_WIN_64_PATH%\%arg%\*.exp
del %BIN_WIN_64_PATH%\%arg%\*.lib
del %BIN_WIN_64_PATH%\%arg%\*.ilk
del %BIN_WIN_64_PATH%\%arg%\log.txt

goto :eof

:removeSlnJunkCPP

set arg=%1

del .\%arg%\*.sdf
del .\%arg%\*.VC.db
del /A:H .\%arg%\*.docstates.suo
del /A:A .\%arg%\*.suo
del .\%arg%\log.txt
rd /s /q .\%arg%\ipch
rd /s /q .\%arg%\.vs

goto :eof

:removeProjJunkCPP

set arg=%1

del .\%arg%\*.user
del .\%arg%\log.txt
rd /s /q .\%arg%\Debug
rd /s /q .\%arg%\Release
rd /s /q .\%arg%\x64\Debug
rd /s /q .\%arg%\x64\Release
rd .\%arg%\x64

goto :eof

:: Function will clear all temporary files for C# project.

:removeBinJunkCSharp

set arg=%1

del %BIN_WIN_32_PATH%\%arg%\*.pdb
del %BIN_WIN_32_PATH%\%arg%\*.vshost.exe
del %BIN_WIN_32_PATH%\%arg%\*.manifest
del %BIN_WIN_32_PATH%\%arg%\log.txt

del %BIN_WIN_64_PATH%\%arg%\*.pdb
del %BIN_WIN_64_PATH%\%arg%\*.vshost.exe
del %BIN_WIN_64_PATH%\%arg%\*.manifest
del %BIN_WIN_64_PATH%\%arg%\log.txt

goto :eof

:removeSlnJunkCSharp

set arg=%1

del /A:H .\%arg%\*.docstates.suo
del /A:A .\%arg%\*.suo
del .\%arg%\*.userprefs

goto :eof

:removeProjJunkCSharp

set arg=%1

rd /s /q %SRC_PATH%\%arg%\obj

goto :eof