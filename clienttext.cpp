#include "faucnet.cpp"

int main()
{
    if(faucnet_init() < 0)
        return 0;
    auto myself_sock = udp_bind(0);
    
    write_string(myself_sock, "P-please respond");
    std::cout << "CLIENT: Sending message to server.\n";
    udp_send(myself_sock, "127.0.0.1", 420);
    std::cout << "CLIENT: Awaiting response...\n";
    
    auto starttime = SDL_GetTicks();
    // wait 60 seconds for an in message on in_sock and put it into in_sock's buffer
    while(udp_receive(myself_sock)==false and  SDL_GetTicks()-starttime < 60*1000);
    
    std::cout << "CLIENT: Got a response!\n";
    auto numbytes = socket_receivebuffer_size(myself_sock);
    auto out_ip = socket_remote_ip(myself_sock);
    auto port = socket_remote_port(myself_sock);
    std::cout << "CLIENT: Size: " << numbytes << " IP: " << out_ip << " Port: " << port << "\n";
    std::cout << "CLIENT: Message: " << read_string(myself_sock, numbytes) << "\n";
    
    faucnet_exit();
    return 0;
}
