#include <SDL2/SDL.h>
#undef main

#include <iostream>

// base
typedef double (*DLLSTARTUP)(); // void:
DLLSTARTUP dllStartup;
typedef double (*DLLSHUTDOWN)(); // void:
DLLSHUTDOWN dllShutdown;

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
typedef void  (*SOCKET_DESTROY)(double); // void: tcpsocket|acceptor
SOCKET_DESTROY socket_destroy;
typedef void  (*SOCKET_DESTROY_ABORTIVE)(double); // void: tcpsocket|acceptor (non-graceful version)
SOCKET_DESTROY_ABORTIVE socket_destroy_abortive;

// data transfer
typedef double (*TCP_RECEIVE)(double, double); // wassuccessful: tcpsocket, numbytes
TCP_RECEIVE tcp_receive;
typedef double (*TCP_RECEIVE_AVAILABLE)(double); // size: tcpsocket
TCP_RECEIVE_AVAILABLE tcp_receive_available;
typedef double (*TCP_EOF)(double); // connectionisclosed: tcpsocket
TCP_EOF tcp_eof;
typedef void  (*SOCKET_SEND)(double); // void: socket
SOCKET_SEND socket_send;
typedef double (*SOCKET_SENDBUFFER_SIZE)(double); // size: socket
SOCKET_SENDBUFFER_SIZE socket_sendbuffer_size;
typedef void  (*SOCKET_SENDBUFFER_LIMIT)(double, double); // void: socket, size
SOCKET_SENDBUFFER_LIMIT socket_sendbuffer_limit;
typedef double (*SOCKET_RECEIVEBUFFER_SIZE)(double); // size: socket
SOCKET_RECEIVEBUFFER_SIZE socket_receivebuffer_size;
typedef double (*UDP_SEND)(double, char *, double); // succeededlimit: udpsocket|buffer, hostname/ip, port
UDP_SEND udp_send;
typedef double (*UDP_RECEIVE)(double); // receivedanything: udpsocket
UDP_RECEIVE udp_receive;

// buffer manipulation
typedef double (*BUFFER_CREATE)(); // buffer
BUFFER_CREATE buffer_create;
typedef void   (*BUFFER_DESTROY)(double); // void: buffer
BUFFER_DESTROY buffer_destroy;
typedef void   (*BUFFER_CLEAR)(double); // void: buffer
BUFFER_CLEAR buffer_clear;
typedef double (*BUFFER_SIZE)(double); // size: buffer
BUFFER_SIZE buffer_size;
typedef double (*BUFFER_BYTES_LEFT)(double); // size: buffer|socketReceiveBuf
BUFFER_BYTES_LEFT buffer_bytes_left;
typedef void   (*BUFFER_SET_READPOS)(double); // void: buffer|socketReceiveBuf
BUFFER_SET_READPOS buffer_set_readpos;

typedef void   (*WRITE_UBYTE)(double, double); // void: buffer|SocketSendBuf, real
WRITE_UBYTE write_ubyte;
typedef void   (*WRITE_BYTE)(double, double); // void: buffer|SocketSendBuf, real
WRITE_BYTE write_byte;
typedef void   (*WRITE_USHORT)(double, double); // void: buffer|SocketSendBuf, real
WRITE_USHORT write_ushort;
typedef void   (*WRITE_SHORT)(double, double); // void: buffer|SocketSendBuf, real
WRITE_SHORT write_short;
typedef void   (*WRITE_UINT)(double, double); // void: buffer|SocketSendBuf, real
WRITE_UINT write_uint;
typedef void   (*WRITE_INT)(double, double); // void: buffer|SocketSendBuf, real
WRITE_INT write_int;
typedef void   (*WRITE_FLOAT)(double, double); // void: buffer|SocketSendBuf, real
WRITE_FLOAT write_float;
typedef void   (*WRITE_DOUBLE)(double, double); // void: buffer|SocketSendBuf, real
WRITE_DOUBLE write_double;
typedef void   (*WRITE_STRING)(double, char *); // void: buffer|SocketSendBuf, string
WRITE_STRING write_string;
typedef void   (*WRITE_BUFFER)(double, double); // void: target, source
WRITE_BUFFER write_buffer;
typedef double (*WRITE_BUFFER_PART)(double, double, double); // actualsize: target, source, size
WRITE_BUFFER_PART write_buffer_part;

typedef double (*READ_UBYTE)(double); // value: buffer|SocketSendBuf
READ_UBYTE read_ubyte;
typedef double (*READ_BYTE)(double); // value: buffer|SocketSendBuf
READ_BYTE read_byte;
typedef double (*READ_USHORT)(double); // value: buffer|SocketSendBuf
READ_USHORT read_ushort;
typedef double (*READ_SHORT)(double); // value: buffer|SocketSendBuf
READ_SHORT read_short;
typedef double (*READ_UINT)(double); // value: buffer|SocketSendBuf
READ_UINT read_uint;
typedef double (*READ_INT)(double); // value: buffer|SocketSendBuf
READ_INT read_int;
typedef double (*READ_FLOAT)(double); // value: buffer|SocketSendBuf
READ_FLOAT read_float;
typedef double (*READ_DOUBLE)(double); // value: buffer|SocketSendBuf
READ_DOUBLE read_double;
typedef char * (*READ_STRING)(double, double); // string: buffer|SocketSendBuf, size
READ_STRING read_string;

// endian
typedef void   (*SET_LITTLE_ENDIAN_GLOBAL)(double); // void: buffer|SocketSendBuf, size
SET_LITTLE_ENDIAN_GLOBAL set_little_endian_global;
typedef void   (*SET_LITTLE_ENDIAN)(double, double); // void: buffer|SocketSendBuf, size
SET_LITTLE_ENDIAN set_little_endian;

// hostname
typedef double (*IP_LOOKUP_CREATE)(char *); // lookup: hostname
IP_LOOKUP_CREATE ip_lookup_create;
typedef double (*IPV4_LOOKUP_CREATE)(char *); // lookup: hostname
IPV4_LOOKUP_CREATE ipv4_lookup_create;
typedef double (*IPV6_LOOKUP_CREATE)(char *); // lookup: hostname
IPV6_LOOKUP_CREATE ipv6_lookup_create;

typedef double (*IP_LOOKUP_READY)(double); // isfinished: lookup
IP_LOOKUP_READY ip_lookup_ready;
typedef double (*IP_LOOKUP_HAS_NEXT)(double); // continues: lookup
IP_LOOKUP_HAS_NEXT ip_lookup_has_next;
typedef char * (*IP_LOOKUP_NEXT_RESULT)(double); // ip: lookup
IP_LOOKUP_NEXT_RESULT ip_lookup_next_result;
typedef void   (*IP_LOOKUP_DESTROY)(double); // continues: lookup
IP_LOOKUP_DESTROY ip_lookup_destroy;

typedef double (*IP_IS_V4)(char *); // whether: ip
IP_IS_V4 ip_is_v4;
typedef double (*IP_IS_V6)(char *); // whether: ip
IP_IS_V6 ip_is_v6;

// misc
typedef char * (*SOCKET_REMOTE_IP)(double); // ip: socket
SOCKET_REMOTE_IP socket_remote_ip;
typedef double (*SOCKET_LOCAL_PORT)(double); // port: socket
SOCKET_LOCAL_PORT socket_local_port;
typedef double (*SOCKET_REMOTE_PORT)(double); // port: socket
SOCKET_REMOTE_PORT socket_remote_port;
typedef double (*SOCKET_HAS_ERROR)(double); // haserror: socket|acceptor
SOCKET_HAS_ERROR socket_has_error;
typedef char * (*SOCKET_ERROR)(double); // error: socket|acceptor
SOCKET_ERROR socket_error;


int faucnet_init()
{
    auto obj = SDL_LoadObject("Faucet Networking.dll");
    if(obj == NULL)
    {
        std::cout << SDL_GetError();
        return -1;
    }
    // init
    dllStartup      = (DLLSTARTUP)  SDL_LoadFunction(obj, "dllStartup");
    dllShutdown    = (DLLSHUTDOWN)  SDL_LoadFunction(obj, "dllShutdown");
    // connection
    tcp_connect             = (TCP_CONNECT)             SDL_LoadFunction(obj, "tcp_connect");
    tcp_listen              = (TCP_LISTEN)              SDL_LoadFunction(obj, "tcp_listen");
    udp_bind                = (UDP_BIND)                SDL_LoadFunction(obj, "udp_bind");
    socket_connecting       = (SOCKET_CONNECTING)       SDL_LoadFunction(obj, "socket_connecting");
    socket_accept           = (SOCKET_ACCEPT)           SDL_LoadFunction(obj, "socket_accept");
    socket_destroy          = (SOCKET_DESTROY)          SDL_LoadFunction(obj, "socket_destroy");
    socket_destroy_abortive = (SOCKET_DESTROY_ABORTIVE) SDL_LoadFunction(obj, "socket_destroy_abortive");
    // senc/receive
    tcp_receive                 = (TCP_RECEIVE)                 SDL_LoadFunction(obj, "tcp_receive");
    tcp_receive_available       = (TCP_RECEIVE_AVAILABLE)       SDL_LoadFunction(obj, "tcp_receive_available");
    tcp_eof                     = (TCP_EOF)                     SDL_LoadFunction(obj, "tcp_eof");
    socket_send                 = (SOCKET_SEND)                 SDL_LoadFunction(obj, "socket_send");
    socket_sendbuffer_size      = (SOCKET_SENDBUFFER_SIZE)      SDL_LoadFunction(obj, "socket_sendbuffer_size");
    socket_sendbuffer_limit     = (SOCKET_SENDBUFFER_LIMIT)     SDL_LoadFunction(obj, "socket_sendbuffer_limit");
    socket_receivebuffer_size   = (SOCKET_RECEIVEBUFFER_SIZE)   SDL_LoadFunction(obj, "socket_receivebuffer_size");
    udp_send                    = (UDP_SEND)                    SDL_LoadFunction(obj, "udp_send");
    udp_receive                 = (UDP_RECEIVE)                 SDL_LoadFunction(obj, "udp_receive");
    // buffer
    buffer_create       = (BUFFER_CREATE)       SDL_LoadFunction(obj, "buffer_create");
    buffer_destroy      = (BUFFER_DESTROY)      SDL_LoadFunction(obj, "buffer_destroy");
    buffer_clear        = (BUFFER_CLEAR)        SDL_LoadFunction(obj, "buffer_clear");
    buffer_size         = (BUFFER_SIZE)         SDL_LoadFunction(obj, "buffer_size");
    buffer_bytes_left   = (BUFFER_BYTES_LEFT)   SDL_LoadFunction(obj, "buffer_bytes_left");
    buffer_set_readpos  = (BUFFER_SET_READPOS)  SDL_LoadFunction(obj, "buffer_set_readpos");
    // write
    write_ubyte         = (WRITE_UBYTE)         SDL_LoadFunction(obj, "write_ubyte");
    write_byte          = (WRITE_BYTE)          SDL_LoadFunction(obj, "write_byte");
    write_ushort        = (WRITE_USHORT)        SDL_LoadFunction(obj, "write_ushort");
    write_short         = (WRITE_SHORT)         SDL_LoadFunction(obj, "write_short");
    write_uint          = (WRITE_UINT)          SDL_LoadFunction(obj, "write_uint");
    write_int           = (WRITE_INT)           SDL_LoadFunction(obj, "write_int");
    write_float         = (WRITE_FLOAT)         SDL_LoadFunction(obj, "write_float");
    write_double        = (WRITE_DOUBLE)        SDL_LoadFunction(obj, "write_double");
    write_string        = (WRITE_STRING)        SDL_LoadFunction(obj, "write_string");
    write_buffer        = (WRITE_BUFFER)        SDL_LoadFunction(obj, "write_buffer");
    write_buffer_part   = (WRITE_BUFFER_PART)   SDL_LoadFunction(obj, "write_buffer_part");
    // read
    read_ubyte  = (READ_UBYTE)  SDL_LoadFunction(obj, "read_ubyte");
    read_byte   = (READ_BYTE)   SDL_LoadFunction(obj, "read_byte");
    read_ushort = (READ_USHORT) SDL_LoadFunction(obj, "read_ushort");
    read_short  = (READ_SHORT)  SDL_LoadFunction(obj, "read_short");
    read_uint   = (READ_UINT)   SDL_LoadFunction(obj, "read_uint");
    read_int    = (READ_INT)    SDL_LoadFunction(obj, "read_int");
    read_float  = (READ_FLOAT)  SDL_LoadFunction(obj, "read_float");
    read_double = (READ_DOUBLE) SDL_LoadFunction(obj, "read_double");
    read_string = (READ_STRING) SDL_LoadFunction(obj, "read_string");
    // endian
    set_little_endian_global    = (SET_LITTLE_ENDIAN_GLOBAL)    SDL_LoadFunction(obj, "set_little_endian_global");
    set_little_endian           = (SET_LITTLE_ENDIAN)           SDL_LoadFunction(obj, "set_little_endian");
    // hostname
    ip_lookup_create    = (IP_LOOKUP_CREATE)    SDL_LoadFunction(obj, "ip_lookup_create");
    ipv4_lookup_create  = (IPV4_LOOKUP_CREATE)  SDL_LoadFunction(obj, "ipv4_lookup_create");
    ipv6_lookup_create  = (IPV6_LOOKUP_CREATE)  SDL_LoadFunction(obj, "ipv6_lookup_create");
    
    ip_lookup_ready         = (IP_LOOKUP_READY)         SDL_LoadFunction(obj, "ip_lookup_ready");
    ip_lookup_has_next      = (IP_LOOKUP_HAS_NEXT)      SDL_LoadFunction(obj, "ip_lookup_has_next");
    ip_lookup_next_result   = (IP_LOOKUP_NEXT_RESULT)   SDL_LoadFunction(obj, "ip_lookup_next_result");
    ip_lookup_destroy       = (IP_LOOKUP_DESTROY)       SDL_LoadFunction(obj, "ip_lookup_destroy");
    
    ip_is_v4  = (IP_IS_V4)  SDL_LoadFunction(obj, "ip_is_v4");
    ip_is_v6  = (IP_IS_V6)  SDL_LoadFunction(obj, "ip_is_v6");
    // misc
    socket_remote_ip    = (SOCKET_REMOTE_IP)    SDL_LoadFunction(obj, "socket_remote_ip");
    socket_local_port   = (SOCKET_LOCAL_PORT)   SDL_LoadFunction(obj, "socket_local_port");
    socket_remote_port  = (SOCKET_REMOTE_PORT)  SDL_LoadFunction(obj, "socket_remote_port");
    socket_has_error    = (SOCKET_HAS_ERROR)    SDL_LoadFunction(obj, "socket_has_error");
    socket_error        = (SOCKET_ERROR)        SDL_LoadFunction(obj, "socket_error");
    
    
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
