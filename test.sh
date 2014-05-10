#!/bin/bash

source='benetnasch.cpp'
cflags='-g -std=c++11 -ggdb -Wall -pedantic -mconsole -I/c/mingw/include/ -Iinclude -L/c/mingw/lib/'
linker='-lmingw32 -lSDL2main -lSDL2_image -lSDL2'

cmd="g++ $source $cflags $linker"

run='./a.exe'

echo
echo $cmd
echo

eval $cmd

if [ "$1" == "-r" ]; then
    eval $run
fi
