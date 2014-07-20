#!/bin/bash

source='benetnasch.cpp'
cflags='-g -std=c++11 -ggdb -Wall -pedantic -mconsole -I/c/mingw32/include/ -Iinclude -L/c/mingw32/lib/'
linker='-lmingw32 -static-libstdc++ -static-libgcc -lSDL2main -lSDL2_image -lSDL2 -lmingw32 -lSDL2main -lSDL2 -lgdi32 -lwinmm -limm32 -lole32 -loleaut32 -lversion -static'

run='./a.exe'

if [ "$1" == "-t" ]; then
    cmd="g++ $source $cflags -D TESTS=1 $linker"
    eval $cmd
    eval $run
else
    cmd="g++ $source $cflags $linker"
    eval $cmd
    if [ "$1" == "-r" ]; then
        eval $run
    fi
fi

