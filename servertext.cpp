#include "faucnet.cpp"

int main()
{
    if(faucnet_init() < 0)
        return 0;
    auto in_sock = udp_bind(420);
    std::cout << "SERVER: Bound input socket on port " << socket_local_port(in_sock) << "\n";
    std::cout << "SERVER: Waiting for connection...\n";
    
    auto starttime = SDL_GetTicks();
    // wait 60 seconds for an in message on in_sock and put it into in_sock's buffer
    while(udp_receive(in_sock)==false and  SDL_GetTicks()-starttime < 60*1000);
    
    std::cout << "SERVER: Got a connection!\n";
    auto numbytes = socket_receivebuffer_size(in_sock);
    auto out_ip = socket_remote_ip(in_sock);
    auto port = socket_remote_port(in_sock);
    std::cout << "SERVER: Size: " << numbytes << " IP: " << out_ip << " Port: " << port << "\n";
    std::cout << "SERVER: Message: " << read_string(in_sock, numbytes) << "\n";
    //  output("Size: ${numbytes} IP: ${out_ip} Port: ${port}\n");
    write_string(in_sock, "Hello, world!\0");
    std::cout << "SERVER: Responding to client.\n";
    udp_send(in_sock, out_ip, port);

    faucnet_exit();
    return 0;
}
