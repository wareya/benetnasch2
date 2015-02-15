#define USE_SOURCE_HEADER_FAUCNET
#include "faucnet.hpp"

#include <SDL2/SDL.h>
#undef main

#include <iostream>
#include <string>
#include <cstdint>

int faucnet_init()
{
    std::string path("");
    path += SDL_GetBasePath();
    #ifdef __MINGW32__
    path += "Faucet Networking.dll";
    #else
    path += "Faucet Networking.so";
    #endif
    auto obj = SDL_LoadObject(path.data());
    if(obj == NULL)
    {
        std::cout << SDL_GetError() << "\n";
        return -1;
    }
    // init
    dllStartup      = (DLLSTARTUP)  (intptr_t)SDL_LoadFunction(obj, "dllStartup");
    dllShutdown     = (DLLSHUTDOWN) (intptr_t)SDL_LoadFunction(obj, "dllShutdown");
    // connection
    tcp_connect             = (TCP_CONNECT)             (intptr_t)SDL_LoadFunction(obj, "tcp_connect");
    tcp_listen              = (TCP_LISTEN)              (intptr_t)SDL_LoadFunction(obj, "tcp_listen");
    udp_bind                = (UDP_BIND)                (intptr_t)SDL_LoadFunction(obj, "udp_bind");
    socket_connecting       = (SOCKET_CONNECTING)       (intptr_t)SDL_LoadFunction(obj, "socket_connecting");
    socket_accept           = (SOCKET_ACCEPT)           (intptr_t)SDL_LoadFunction(obj, "socket_accept");
    socket_destroy          = (SOCKET_DESTROY)          (intptr_t)SDL_LoadFunction(obj, "socket_destroy");
    socket_destroy_abortive = (SOCKET_DESTROY_ABORTIVE) (intptr_t)SDL_LoadFunction(obj, "socket_destroy_abortive");
    // senc/receive
    tcp_receive                 = (TCP_RECEIVE)                 (intptr_t)SDL_LoadFunction(obj, "tcp_receive");
    tcp_receive_available       = (TCP_RECEIVE_AVAILABLE)       (intptr_t)SDL_LoadFunction(obj, "tcp_receive_available");
    tcp_eof                     = (TCP_EOF)                     (intptr_t)SDL_LoadFunction(obj, "tcp_eof");
    socket_send                 = (SOCKET_SEND)                 (intptr_t)SDL_LoadFunction(obj, "socket_send");
    socket_sendbuffer_size      = (SOCKET_SENDBUFFER_SIZE)      (intptr_t)SDL_LoadFunction(obj, "socket_sendbuffer_size");
    socket_sendbuffer_limit     = (SOCKET_SENDBUFFER_LIMIT)     (intptr_t)SDL_LoadFunction(obj, "socket_sendbuffer_limit");
    socket_receivebuffer_size   = (SOCKET_RECEIVEBUFFER_SIZE)   (intptr_t)SDL_LoadFunction(obj, "socket_receivebuffer_size");
    udp_send                    = (UDP_SEND)                    (intptr_t)SDL_LoadFunction(obj, "udp_send");
    udp_receive                 = (UDP_RECEIVE)                 (intptr_t)SDL_LoadFunction(obj, "udp_receive");
    // buffer
    buffer_create       = (BUFFER_CREATE)       (intptr_t)SDL_LoadFunction(obj, "buffer_create");
    buffer_destroy      = (BUFFER_DESTROY)      (intptr_t)SDL_LoadFunction(obj, "buffer_destroy");
    buffer_clear        = (BUFFER_CLEAR)        (intptr_t)SDL_LoadFunction(obj, "buffer_clear");
    buffer_size         = (BUFFER_SIZE)         (intptr_t)SDL_LoadFunction(obj, "buffer_size");
    buffer_bytes_left   = (BUFFER_BYTES_LEFT)   (intptr_t)SDL_LoadFunction(obj, "buffer_bytes_left");
    buffer_set_readpos  = (BUFFER_SET_READPOS)  (intptr_t)SDL_LoadFunction(obj, "buffer_set_readpos");
    // write
    write_ubyte         = (WRITE_UBYTE)         (intptr_t)SDL_LoadFunction(obj, "write_ubyte");
    write_byte          = (WRITE_BYTE)          (intptr_t)SDL_LoadFunction(obj, "write_byte");
    write_ushort        = (WRITE_USHORT)        (intptr_t)SDL_LoadFunction(obj, "write_ushort");
    write_short         = (WRITE_SHORT)         (intptr_t)SDL_LoadFunction(obj, "write_short");
    write_uint          = (WRITE_UINT)          (intptr_t)SDL_LoadFunction(obj, "write_uint");
    write_int           = (WRITE_INT)           (intptr_t)SDL_LoadFunction(obj, "write_int");
    write_float         = (WRITE_FLOAT)         (intptr_t)SDL_LoadFunction(obj, "write_float");
    write_double        = (WRITE_DOUBLE)        (intptr_t)SDL_LoadFunction(obj, "write_double");
    write_string        = (WRITE_STRING)        (intptr_t)SDL_LoadFunction(obj, "write_string");
    write_buffer        = (WRITE_BUFFER)        (intptr_t)SDL_LoadFunction(obj, "write_buffer");
    write_buffer_part   = (WRITE_BUFFER_PART)   (intptr_t)SDL_LoadFunction(obj, "write_buffer_part");
    // read
    read_ubyte  = (READ_UBYTE)  (intptr_t)SDL_LoadFunction(obj, "read_ubyte");
    read_byte   = (READ_BYTE)   (intptr_t)SDL_LoadFunction(obj, "read_byte");
    read_ushort = (READ_USHORT) (intptr_t)SDL_LoadFunction(obj, "read_ushort");
    read_short  = (READ_SHORT)  (intptr_t)SDL_LoadFunction(obj, "read_short");
    read_uint   = (READ_UINT)   (intptr_t)SDL_LoadFunction(obj, "read_uint");
    read_int    = (READ_INT)    (intptr_t)SDL_LoadFunction(obj, "read_int");
    read_float  = (READ_FLOAT)  (intptr_t)SDL_LoadFunction(obj, "read_float");
    read_double = (READ_DOUBLE) (intptr_t)SDL_LoadFunction(obj, "read_double");
    read_string = (READ_STRING) (intptr_t)SDL_LoadFunction(obj, "read_string");
    // endian
    set_little_endian_global    = (SET_LITTLE_ENDIAN_GLOBAL)    (intptr_t)SDL_LoadFunction(obj, "set_little_endian_global");
    set_little_endian           = (SET_LITTLE_ENDIAN)           (intptr_t)SDL_LoadFunction(obj, "set_little_endian");
    // hostname
    ip_lookup_create    = (IP_LOOKUP_CREATE)    (intptr_t)SDL_LoadFunction(obj, "ip_lookup_create");
    ipv4_lookup_create  = (IPV4_LOOKUP_CREATE)  (intptr_t)SDL_LoadFunction(obj, "ipv4_lookup_create");
    ipv6_lookup_create  = (IPV6_LOOKUP_CREATE)  (intptr_t)SDL_LoadFunction(obj, "ipv6_lookup_create");
    
    ip_lookup_ready         = (IP_LOOKUP_READY)         (intptr_t)SDL_LoadFunction(obj, "ip_lookup_ready");
    ip_lookup_has_next      = (IP_LOOKUP_HAS_NEXT)      (intptr_t)SDL_LoadFunction(obj, "ip_lookup_has_next");
    ip_lookup_next_result   = (IP_LOOKUP_NEXT_RESULT)   (intptr_t)SDL_LoadFunction(obj, "ip_lookup_next_result");
    ip_lookup_destroy       = (IP_LOOKUP_DESTROY)       (intptr_t)SDL_LoadFunction(obj, "ip_lookup_destroy");
    
    ip_is_v4  = (IP_IS_V4)  (intptr_t)SDL_LoadFunction(obj, "ip_is_v4");
    ip_is_v6  = (IP_IS_V6)  (intptr_t)SDL_LoadFunction(obj, "ip_is_v6");
    // misc
    socket_remote_ip    = (SOCKET_REMOTE_IP)    (intptr_t)SDL_LoadFunction(obj, "socket_remote_ip");
    socket_local_port   = (SOCKET_LOCAL_PORT)   (intptr_t)SDL_LoadFunction(obj, "socket_local_port");
    socket_remote_port  = (SOCKET_REMOTE_PORT)  (intptr_t)SDL_LoadFunction(obj, "socket_remote_port");
    socket_has_error    = (SOCKET_HAS_ERROR)    (intptr_t)SDL_LoadFunction(obj, "socket_has_error");
    socket_error        = (SOCKET_ERROR)        (intptr_t)SDL_LoadFunction(obj, "socket_error");
    
    read_hex = (READ_HEX) (intptr_t)SDL_LoadFunction(obj, "read_hex");
    
    
    if (!(dllStartup && dllShutdown
       && tcp_connect && tcp_listen && udp_bind && socket_connecting && socket_accept && socket_destroy && socket_destroy_abortive
       && tcp_receive && tcp_receive_available && tcp_eof
       && socket_send && socket_sendbuffer_size && socket_sendbuffer_limit && socket_receivebuffer_size
       && udp_send && udp_receive
       && buffer_create && buffer_destroy && buffer_clear && buffer_size && buffer_bytes_left && buffer_set_readpos
       && write_ubyte && write_byte && write_ushort && write_short && write_uint && write_int && write_float && write_double && write_string
       && write_buffer && write_buffer_part
       && read_ubyte && read_byte && read_ushort && read_short && read_uint && read_int && read_float && read_double && read_string
       && set_little_endian_global && set_little_endian
       && ip_lookup_create && ipv4_lookup_create && ipv6_lookup_create
       && ip_lookup_ready && ip_lookup_has_next && ip_lookup_next_result && ip_lookup_destroy
       && ip_is_v4 && ip_is_v6
       && socket_remote_ip && socket_local_port && socket_remote_port && socket_has_error && socket_error))
    {
        std::cout << SDL_GetError();
        return -1;
    }
    dllStartup();
    return 0;
}

void faucnet_exit()
{
    dllShutdown();
}

#ifdef FAUCNET_TEST
int main()
{
    if(faucnet_init() == 0)
        std::cout << "Loaded functions successfully\n";
    auto sock = tcp_connect("google.com", 80);
    std::cout << "Waiting for connection...\n";
    while(socket_connecting(sock));
    std::cout << "Connected!\n";
    faucnet_exit();
    return 0;
}
#endif

