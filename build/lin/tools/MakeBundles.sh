#!/bin/bash

# Clear screen
clear

# Rebuild solution
xbuild ./../../common/tools/Tools.sln

# Go to bin destination
cd ./../../../bin/win/tools

# Get current directory
BIN_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

# Add assemblies to mono path
export MONO_PATH=$MONO_PATH:$BIN_DIR

# Make Template bundle with static flag
mkbundle -c -o Template.c -oo Template.o --deps --static Template.exe

# Build Template bundle
cc -o Temlate -Wall Template.c `pkg-config --cflags --libs mono-2` Template.o

# Delete temporary files
rm *.pdb *.mdb *.o *.c