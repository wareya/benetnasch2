#include "src/network.hpp"

enum {
    MESSAGE_STRING
};

void process_message_string(Net::Connection * connection, double buffer)
{
    std::cout << "SERVER: Got a string size: " << buffer_size(buffer) << " IP: " << connection->hostname << " Port: " << connection->port << "\n";
    std::cout << read_string(buffer, buffer_size(buffer)) << "\n";
    
    auto response = buffer_create();
    write_string(response, (char *)"Hello, client!");
    send(connection, 0, MESSAGE_STRING, response);
    buffer_destroy(response);
}

int main()
{
    if(faucnet_init() < 0)
        return 0;
    Net::init(420);
    std::cout << "SERVER: Bound input socket on port " << socket_local_port(Net::local_socket) << "\n";
    std::cout << "SERVER: Waiting for connection...\n";
    
    Net::assign ( 0, MESSAGE_STRING, &process_message_string );
    
    auto starttime = SDL_GetTicks();
    
    while(SDL_GetTicks()-starttime < 60*1000)
    {
        Net::think();
        SDL_Delay(50);
    }
    
    faucnet_exit();
    return 0;
}
