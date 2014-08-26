#!/bin/bash

source='benetnasch.cpp'
forceinclude="`sdl2-config --prefix`"
sdliflags='`sdl2-config --cflags`'
sdllflags='`sdl2-config --static-libs` -lSDL2_image -static'
sdllflagsstatic='`pkg-config SDL2_image --static-libs` -lSDL2_image'
cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"

if hash sdl2-config; then
    cat /dev/null;
else
    echo "Could not find sdl2-config. Is SDL2 installed correctly? Aborting."
    exit 1
fi

echo ""
echo "Check sdl2-config --prefix:"
echo "${forceinclude}"
echo "Please edit sdl2-config if ${forceinclude}/include/SDL2/SDL.h does not exist."
echo "Also, if you get an 'XCClinker' error, remove that flag from sdl2_config."
echo ""

dflags='-O0 -g -ggdb -mconsole'

fflags='-O3'
mflags='-O3 -msse -msse2' # modern x86 optimizations
iflags='-O3 -msse -msse2 -mssse3 -msse4.1' # aggressive intel x86 optimizations
aflags='-O3 -msse -msse2 -msse2avx' # aggressive amd x86 optimizations

tflags='-D TESTS=1'
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '
sflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_NORENDER '

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
elif [ "$1" == "-s" ]; then
    cmd="$cmd $sflags"
elif [ "$1" == "-c" ]; then
    cmd="$cmd ${@:2}";
fi

echo $cmd

run='./a.exe'

eval $cmd
if [ "$1" == "-t" ]; then    
    eval $run
fi

