// g++ --std=c++11 faucnetcrush.cpp -lSDL2 -mconsole; ./a.exe

#include "src/blib/faucnet.cpp"
#include "SDL2/SDL.h"
#include <stdio.h>

int main()
{
    if(faucnet_init() < 0)
        return 0;
    
    puts("Starting.");
    double local_socket = udp_bind(2580);
    
    write_uint(local_socket, 10);
    udp_send(local_socket, "127.0.0.1", 2580);
    
    SDL_Delay(50);
    
    while(udp_receive(local_socket))
    {
        puts("Got a response!");
    }
    
    double temp1 = buffer_create();
    double temp2 = buffer_create();
    
    write_uint(local_socket, 10);
    write_uint(local_socket, 10);
    
    write_buffer(temp1, temp1);
    write_buffer(temp1, temp1);
    
    udp_send(local_socket, "127.0.0.1", 2580);
    
    buffer_destroy(temp1);
    buffer_destroy(temp2);
    SDL_Delay(50);
    
    while(udp_receive(local_socket))
    {
        puts("That a fact?");
    }
    
}
