#!/bin/bash


source=(
 "src/blib.cpp"
 "src/bengine.cpp"
 "src/components.cpp"
 "src/entity.cpp"
 "src/input.cpp"
 "src/maps.cpp"
 "src/benetnasch.cpp"
 "src/physics.cpp"
 "src/network.cpp"
 "src/channelmap.cpp"
 "src/serverplayer.cpp"
 "src/components/backgrounddrawable.cpp"
 "src/components/boxdrawable.cpp"
 "src/components/bullet.cpp"
 "src/components/character.cpp"
 "src/components/primitives.cpp"
 "src/components/componentlists.cpp"
 "src/components/player.cpp"
 "src/physics/characters.cpp"
 "src/physics/bullets.cpp"
 "src/physics/subroutines.cpp")

codeset=""

if [ "$1" == "server" ] || [ "$1" == "-s" ] || [ "$2" == "server" ]; then
    if [ "$OSTYPE" == "msys" ]; then
        executable="server.exe"
    else
        executable="server.out"
    fi
    source+=('src/bootserver.cpp')
    source+=('src/server/think.cpp')
    source+=('src/server/nethandlers.cpp')
    codeset=" -DSERVER"
    mkdir -p srvobj
    mkdir -p srvobj/physics
    mkdir -p srvobj/rendering
    mkdir -p srvobj/components
    mkdir -p srvobj/client
    mkdir -p srvobj/server
else
    mkdir -p obj
    mkdir -p obj/physics
    mkdir -p obj/rendering
    mkdir -p obj/components
    mkdir -p obj/client
    mkdir -p obj/server
    if [ "$OSTYPE" == "msys" ]; then
        executable="benetnasch.exe"
    else
        executable="benetnasch.out"
    fi
    source+=('src/bootclient.cpp')
    source+=('src/client/clientdata.cpp')
    source+=('src/client/think.cpp')
    source+=('src/client/nethandlers.cpp')
    source+=('src/client/scripting.cpp')
    source+=('src/client/textinput.cpp')
    source+=('src/rendering/drawbackground.cpp')
    source+=('src/rendering/drawboxes.cpp')
    source+=('src/rendering/drawbullets.cpp')
    source+=('src/rendering/drawcharacterdebug.cpp')
    source+=('src/rendering/drawrotatetextured.cpp')
    source+=('src/rendering/drawscreentext.cpp')
    source+=('src/rendering/drawtextured.cpp')
    source+=('src/rendering/drawanimatedtextured.cpp')
    source+=('src/rendering/drawspeedometer.cpp')
    source+=('src/rendering/drawtextwindows.cpp')
    source+=('src/rendering.cpp')
    source+=('src/components/textwindow.cpp')
    source+=('src/components/textureddrawable.cpp')
    source+=('src/components/animatedtextureddrawable.cpp')
    source+=('src/components/rotatingtextureddrawable.cpp')
    source+=('src/samples.cpp')
    codeset=" -DCLIENT"
fi

if [ "$OSTYPE" == "msys" ]; then
    echo "Platform seems to be windows. Report this bug if this is not the case."
    
    forceinclude="`sdl2-config --prefix`"
    sdliflags="`sdl2-config --cflags`"
    sdllflags="`sdl2-config --static-libs` -lSDL2_image -static"
    cflags="-std=c++11 -Wall -pedantic -Iinclude $sdliflags -I${forceinclude}/include"
    linker="-L /usr/lib -static-libstdc++ -static-libgcc $sdllflags -mconsole -mwindows"

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

cflags+="$codeset"

#TODO: DETECT
linker+=' -llua -L. fauxmix.dll'

cmd="g++ -m32 -march=i686 $cflags"

if [ $OSTYPE == "msys" ]; then
    console="-mconsole"
fi

#options
dflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION"
ddflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_MISTAKES"
dddflags="-O0 -g -ggdb $console -D B_NET_DEBUG_CONNECTION -D B_NET_DEBUG_PRINTPACK"

fflags='-O3'
mflags='-O3 -msse -msse2' # modern x86 optimizations
iflags='-O3 -msse -msse2 -mssse3 -msse4.1' # aggressive intel x86 optimizations
aflags='-O3 -msse -msse2 -msse2avx' # aggressive amd x86 optimizations

tflags='-D TESTS=1'
pflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_FRAMESONLY -D B_DEBUG_COREFRAMES '
zflags='-O3 -D B_FRAMELIMIT_DISABLE -D B_DEBUG_NORENDER '

if [ "$1" == "-d" ]; then
    cmd="$cmd $dflags"
elif [ "$1" == "-dd" ]; then
    cmd="$cmd $ddflags"
elif [ "$1" == "-ddd" ]; then
    cmd="$cmd $dddflags"
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
    if [ "$codeset" == " -DSERVER" ]; then
        obj="`echo $i | sed 's-src/-srvobj/-g' | sed 's-.cpp-.o-g'`"
    else
        obj="`echo $i | sed 's-src/-obj/-g' | sed 's-.cpp-.o-g'`"
    fi
    deps=($(gcc -std=c++11 -MM $i | sed -e 's/^\w*.o://' | tr '\n' ' ' | sed -e 's/\\//g' | sed 's/ \+//' | sed 's/ \+/\n/g'))
    for j in "${deps[@]}"
    do
        if test $j -nt $obj; then
            echo "$cmd -c $i -o $obj"
            $cmd -c $i -o $obj
			break
        fi
    done
    objects="$objects $obj"
done
echo "g++ -o $executable $objects $linker"
g++ -m32 -o $executable $objects $linker

echo "done"
