#include "faucnet.cpp"

int main()
{
    if(faucnet_init() < 0)
        return 0;
    auto mybuff = buffer_create();
    
    write_string(mybuff, "Hello, world!\0");
    std::cout << "CLENT: Sending message to server.\n";
    udp_send(mybuff, "127.0.0.1", 420);
    
    faucnet_exit();
    return 0;
}
