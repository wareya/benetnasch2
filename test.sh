#!/bin/bash

if [ "$OSTYPE" == "msys" ]; then
    executable="benetnasch.exe"
else
    executable="benetnasch.out"
fi

mkdir -p obj
mkdir -p obj/physics
mkdir -p obj/rendering
mkdir -p obj/components
source=(
 "src/blib.cpp"
 "src/rendering/drawspeedometer.cpp"
 "src/bengine.cpp"
 "src/components.cpp"
 "src/entity.cpp"
 "src/input.cpp"
 "src/maps.cpp"
 "src/benetnasch.cpp"
 "src/physics.cpp"
 "src/rendering.cpp"
 "src/network.cpp"
 "src/components/backgrounddrawable.cpp"
 "src/components/boxdrawable.cpp"
 "src/components/bullet.cpp"
 "src/components/character.cpp"
 "src/components/primitives.cpp"
 "src/components/rotatingtextureddrawable.cpp"
 "src/components/textureddrawable.cpp"
 "src/components/componentlists.cpp"
 "src/physics/characters.cpp"
 "src/physics/bullets.cpp"
 "src/physics/subroutines.cpp"
 "src/rendering/drawbackground.cpp"
 "src/rendering/drawboxes.cpp"
 "src/rendering/drawbullets.cpp"
 "src/rendering/drawcharacterdebug.cpp"
 "src/rendering/drawrotatetextured.cpp"
 "src/rendering/drawscreentext.cpp"
 "src/rendering/drawtextured.cpp")

if [ "$1" == "server" ] || [ "$1" == "-s" ] || [ "$2" == "server" ]; then
    source+=('src/bootserver.cpp')
else
    source+=('src/bootclient.cpp')
fi

if [ "$OSTYPE" == "msys" ]; then
    echo "Platform seems to be windows. Report this bug if this is not the case."
    
    forceinclude="`sdl2-config --prefix`"
    sdliflags="`sdl2-config --cflags`"
    sdllflags="`sdl2-config --static-libs` -lSDL2_image -static"
    cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"
    linker="-L /usr/lib -static-libstdc++ -static-libgcc $sdllflags"

    if hash sdl2-config; then
        cat /dev/null;
    else
        echo "Could not find sdl2-config. Is SDL2 installed correctly? Aborting."
        exit 1
    fi

    echo ""
    echo "Checking sdl2-config --prefix: ${forceinclude}"
    if [ ! -f "${forceinclude}/lib/libSDL2.a" ]; then
        echo "sdl2-config prefix does not seem to be valid: edit sdl2-config."
        echo "Aborting."
        exit 1
    fi
    echo "Looks okay."
    echo "Also, if you get an 'XCClinker' error, remove that flag from sdl2_config."
    echo ""
else
    echo "Platform seems to be linux. If not, $OSTYPE is wrong."
    
    if hash pkg-config 2>/dev/null; then # prefer pkg-config to sdl2-config
        echo "Using pkg-config for SDL2 compiler flags."
        sdliflags="`pkg-config --cflags sdl2`"
        sdllflags="`pkg-config --libs sdl2` -lSDL2_image"
        cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags"
        linker="-L /usr/lib $sdllflags"
    else
        forceinclude="`sdl2-config --prefix`" # avoid unfortunate packing mistake
        sdliflags="`sdl2-config --cflags`"
        sdllflags="`sdl2-config --libs` -lSDL2_image"
        cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"
        linker="-L /usr/lib $sdllflags"
        if hash sdl2-config; then
            cat /dev/null;
        else
            echo "Could not find sdl2-config. Is SDL2 installed correctly? Aborting."
            exit 1
        fi
        
        echo ""
        echo "Checking sdl2-config --prefix: ${forceinclude}"
        if [ ! -f "${forceinclude}/lib/libSDL2.so" ]; then
            echo "sdl2-config prefix does not seem to be valid: edit sdl2-config."
            echo "Aborting."
            exit 1
        fi
        echo "Looks okay."
    fi
fi


cmd="g++ $cflags"


#options
dflags='-O0 -g -ggdb -mconsole'

fflags='-O3'
mflags='-O3 -msse -msse2' # modern x86 optimizations
iflags='-O3 -msse -msse2 -mssse3 -msse4.1' # aggressive intel x86 optimizations
aflags='-O3 -msse -msse2 -msse2avx' # aggressive amd x86 optimizations

tflags='-D TESTS=1'
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '
zflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_NORENDER '

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
elif [ "$1" == "-z" ]; then
    cmd="$cmd $zflags"
elif [ "$1" == "-s" ]; then
    cmd="$cmd ${@:2}";
elif [ "$1" == "-c" ]; then
    cmd="$cmd ${@:2}";
fi

objects=""

for i in "${source[@]}"
do
    obj="`echo $i | sed 's-src/-obj/-g' | sed 's-.cpp-.o-g'`"
    deps=($(gcc -std=c++11 -MM $i | sed -e 's/^\w*.o://' | tr '\n' ' ' | sed -e 's/\\//g' | sed 's/ \+//' | sed 's/ \+/\n/g'))
    for j in "${deps[@]}"
    do
        if test $j -nt $obj; then
            echo "$cmd -c $j -o $obj"
            $cmd -c $i -o $obj
			break
        fi
    done
    objects="$objects $obj"
done
echo "g++ -o $executable $objects $linker"
g++ -o $executable $objects $linker

echo "done"
