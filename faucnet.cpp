#include <SDL2/SDL.h>
#undef main

#include <iostream>

// connection
typedef double (*TCP_CONNECT)(char *, double); // tcpsocket: hostname/ip, port
TCP_CONNECT tcp_connect;
typedef double (*TCP_LISTEN)(double); // acceptor: port
TCP_LISTEN tcp_listen;
typedef double (*UDP_BIND)(double); // udpsocket: port
UDP_BIND udp_bind;
typedef double (*SOCKET_CONNECTING)(double); // isconnecting: socket (always returns false for udp sockets)
SOCKET_CONNECTING socket_connecting;
typedef double (*SOCKET_ACCEPT)(double); // tcpsocket|errorcode: acceptor (returns negative for nothing to accept)
SOCKET_ACCEPT socket_accept;
typedef double (*SOCKET_DESTROY)(double); // void: tcpsocket|acceptor
SOCKET_DESTROY socket_destroy;
typedef double (*SOCKET_DESTROY_ABORTIVE)(double); // void: tcpsocket|acceptor (non-graceful version)
SOCKET_DESTROY_ABORTIVE socket_destroy_abortive;

// data transfer
typedef double (*TCP_RECEIVE)(double, double); // wassuccessful: tcpsocket, numbytes
TCP_RECEIVE tcp_receive;
typedef double (*TCP_RECEIVE_AVAILABLE)(double); // size: tcpsocket
TCP_RECEIVE_AVAILABLE tcp_receive_available;
typedef double (*TCP_EOF)(double); // connectionisclosed: tcpsocket
TCP_EOF tcp_eof;
typedef double (*SOCKET_SEND)(double); // void: socket
SOCKET_SEND socket_send;
typedef double (*SOCKET_SENDBUFFER_SIZE)(double); // size: socket
SOCKET_SENDBUFFER_SIZE socket_sendbuffer_size;
typedef double (*SOCKET_SENDBUFFER_LIMIT)(double, double); // void: socket, size
SOCKET_SENDBUFFER_LIMIT socket_sendbuffer_limit;
typedef double (*SOCKET_RECEIVEBUFFER_SIZE)(double); // size: socket
SOCKET_RECEIVEBUFFER_SIZE socket_receivebuffer_size;
typedef double (*UDP_SEND)(double, char *, double); // succeededlimit: udpsocket|buffer, hostname/ip, port
UDP_SEND udp_send;
typedef double (*UDP_RECEIVE)(double); // receivedanything: udpsocket
UDP_RECEIVE udp_receive;

// buffer manipulation
typedef double (*BUFFER_CREATE)(); // buffer:
BUFFER_CREATE buffer_create;
typedef double (*BUFFER_DESTROY)(???); // void: buffer
BUFFER_DESTROY buffer_destroy;
typedef double (*BUFFER_CLEAR)(???); // void: buffer
BUFFER_CLEAR buffer_clear;
typedef double (*BUFFER_SIZE)(???); // size: buffer
BUFFER_SIZE buffer_size;
typedef double (*BUFFER_BYTES_LEFT)(???); // size: buffer|socketReceiveBuf
BUFFER_BYTES_LEFT buffer_bytes_left;
typedef double (*BUFFER_SET_READPOS)(???); // void: buffer|socketReceiveBuf
BUFFER_SET_READPOS buffer_set_readpos;



int set_faucnet_functions()
{
    auto obj = SDL_LoadObject("Faucet Networking.dll");
    if(obj == NULL)
    {
        std::cout << SDL_GetError();
        return -1;
    }
    tcp_connect = (TCP_CONNECT) SDL_LoadFunction(obj, "tcp_connect");
    udp_bind    = (UDP_BIND)    SDL_LoadFunction(obj, "udp_send");
    if (tcp_connect && udp_bind == false)
    {
        std::cout << SDL_GetError();
        return -1;
    }
    return 0;
}

#ifdef FAUCNET_TEST
int main(int argc, char *argv[])
{
    if(set_faucnet_functions() == 0)
        std::cout << "Test ran successfully!";
    return 0;
}
#endif
