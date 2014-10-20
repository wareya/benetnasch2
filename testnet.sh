cc="g++ -Wall -Wextra -pedantic -std=c++11"

if [ "$OSTYPE" == "msys" ]; then
    flags="`sdl2-config --cflags` -static-libstdc++ `sdl2-config --static-libs` -lSDL2_image -static -mconsole"
else
    flags="`sdl2-config --cflags` `sdl2-config --libs`"
fi

$cc servertest.cpp src/network.cpp src/blib.cpp $flags -o servertest.exe
$cc clienttest.cpp src/network.cpp src/blib.cpp $flags -o clienttest.exe
./servertest.exe & ./clienttest.exe
