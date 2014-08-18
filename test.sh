#!/bin/bash

source='benetnasch.cpp'
sdliflags='`pkg-config SDL2_image --cflags`'
sdllflags='`pkg-config SDL2_image --libs`'
sdllflagsstatic='`pkg-config SDL2_image --static-libs`'
cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags"
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '
fflags='-O0 -g -ggdb'
rflags='-O3'
tflags='-D TESTS=1'
linker="-L /usr/lib -static-libstdc++ -static-libgcc $sdllflags"

cmd="g++ $source $cflags $linker"

if [ "$1" == "-d" ]; then
    cmd="$cmd $dflags"
elif [ "$1" == "-f" ]; then
    cmd="$cmd $fflags"
elif [ "$1" == "-p" ]; then
    cmd="$cmd $pflags"
elif [ "$1" == "-t" ]; then
    cmd="$cmd $tflags"
fi

run='./a.exe'

eval $cmd
if [ "$1" == "-t" ]; then    
    eval $run
fi

