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
    
    //const char * remote = "192.168.0.162";
    const char * remote = "192.168.0.148";
    //const char * remote = "127.0.0.1";
    
    write_string(local_socket, "Biribiri moshimoshi");
    udp_send(local_socket, remote, 2580);
    
    SDL_Delay(50);
    
    while(udp_receive(local_socket))
    {
        puts("Got a response!");
    }
    
    // even though temp2 is completely unused it affects the reproducability
    double temp1 = buffer_create();
    double temp2 = buffer_create();
    
    // these could be any size I think
    write_string(local_socket, "Hayakara Danabu");
    write_string(local_socket, "");
    
    // if I delete one of these it stops working
    write_buffer(temp1, temp2);
    write_buffer(temp2, temp1);
    
    udp_send(local_socket, remote, 2580);
    
    buffer_destroy(temp1);
    buffer_destroy(temp2);
    SDL_Delay(50);
    
    // send fails reliably on my system, might depend on how faucet networking is compiled.
    while(udp_receive(local_socket))
    {
        puts("That a fact?");
    }
}
