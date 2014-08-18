#!/bin/bash

source='benetnasch.cpp'
sdliflags='`pkg-config SDL2_image --cflags`'
sdllflags='`pkg-config SDL2_image --libs`'
sdllflagsstatic='`pkg-config SDL2_image --static-libs`'
cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags"

dflags='-O0 -g -ggdb'

fflags='-O3'
mflags='-O3 -msse -msse2' # modern x86 optimizations
iflags='-O3 -msse -msse2 -mssse3 -msse4.1' # aggressive intel x86 optimizations
aflags='-O3 -msse -msse2 -msse2avx' # aggressive amd x86 optimizations

tflags='-D TESTS=1'
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '

linker="-L /usr/lib -static-libstdc++ -static-libgcc $sdllflags"

cmd="g++ $source $cflags $linker"

if [ "$1" == "-d" ]; then
    cmd="$cmd $dflags"
elif [ "$1" == "-f" ]; then
    cmd="$cmd $fflags"
elif [ "$1" == "-m" ]; then
    cmd="$cmd $mflags"
elif [ "$1" == "-i" ]; then
    cmd="$cmd $iflags"
elif [ "$1" == "-a" ]; then
    cmd="$cmd $aflags"
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

