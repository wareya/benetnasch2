#include "src/network.hpp"

enum {
    MESSAGE_STRING
};

void process_message_string(Net::Connection * connection, double buffer)
{
    std::cout << "CLIENT: Got a string size: " << buffer_size(buffer) << " IP: " << connection->hostname << " Port: " << connection->port << "\n";
    std::cout << read_string(buffer, buffer_size(buffer)) << "\n";
}
int main()
{
    if(faucnet_init() < 0)
        return 0;
    Net::init(0);
    std::cout << "CLIENT: Bound input socket on port " << socket_local_port(Net::local_socket) << "\n";
    std::cout << "CLIENT: Sending connection...\n";
    
    auto remote = new Net::Connection( "127.0.0.1", 420 );
    Net::connections.push_back(remote);
    Net::assign ( 0, MESSAGE_STRING, &process_message_string );
    remote->send_or_resend_connection_request();
    
    auto starttime = SDL_GetTicks();
    
    while(SDL_GetTicks()-starttime < 60*1000)
    {
        Net::think();
        if ( remote->ready )
        {
            std::cout << "CLIENT: Sending server a message...\n";
            std::cout << "CLIENT: Check stored IP/port: " << remote->hostname << ":" << remote->port << "\n";
            auto b = buffer_create();
            write_string(b, "Hello, server!");
            send(remote, false, 0, b);
            buffer_destroy(b);
            break;
        }
        SDL_Delay(1);
    }
    while(SDL_GetTicks()-starttime < 60*1000)
    {
        Net::think();
        SDL_Delay(50);
    }
    
    faucnet_exit();
    return 0;
}
