@echo off

rd /s /q ..\..\docs\api_help
md ..\..\docs\api_help

cd ..\..\misc\api_docs_gen

..\..\bin\windows\build_tools\doxygen.exe help.cfg