#ifndef INCLUDED_FAUCNET
#define INCLUDED_FAUCNET

#include <SDL2/SDL.h>
#undef main

#include <iostream>

#undef MAYBEEXTERN
#ifdef USE_SOURCE_HEADER_FAUCNET
#define MAYBEEXTERN 
#else
#define MAYBEEXTERN extern
#endif

// base
typedef double (*DLLSTARTUP)(); // void:
MAYBEEXTERN DLLSTARTUP dllStartup;
typedef double (*DLLSHUTDOWN)(); // void:
MAYBEEXTERN DLLSHUTDOWN dllShutdown;

// connection
typedef double (*TCP_CONNECT)(char *, double); // tcpsocket: hostname/ip, port
MAYBEEXTERN TCP_CONNECT tcp_connect;
typedef double (*TCP_LISTEN)(double); // acceptor: port
MAYBEEXTERN TCP_LISTEN tcp_listen;
typedef double (*UDP_BIND)(double); // udpsocket: port
MAYBEEXTERN UDP_BIND udp_bind;
typedef double (*SOCKET_CONNECTING)(double); // isconnecting: socket (always returns false for udp sockets)
MAYBEEXTERN SOCKET_CONNECTING socket_connecting;
typedef double (*SOCKET_ACCEPT)(double); // tcpsocket|errorcode: acceptor (returns negative for nothing to accept)
MAYBEEXTERN SOCKET_ACCEPT socket_accept;
typedef void  (*SOCKET_DESTROY)(double); // void: tcpsocket|acceptor
MAYBEEXTERN SOCKET_DESTROY socket_destroy;
typedef void  (*SOCKET_DESTROY_ABORTIVE)(double); // void: tcpsocket|acceptor (non-graceful version)
MAYBEEXTERN SOCKET_DESTROY_ABORTIVE socket_destroy_abortive;

// data transfer
typedef double (*TCP_RECEIVE)(double, double); // wassuccessful: tcpsocket, numbytes
MAYBEEXTERN TCP_RECEIVE tcp_receive;
typedef double (*TCP_RECEIVE_AVAILABLE)(double); // size: tcpsocket
MAYBEEXTERN TCP_RECEIVE_AVAILABLE tcp_receive_available;
typedef double (*TCP_EOF)(double); // connectionisclosed: tcpsocket
MAYBEEXTERN TCP_EOF tcp_eof;
typedef void  (*SOCKET_SEND)(double); // void: socket
MAYBEEXTERN SOCKET_SEND socket_send;
typedef double (*SOCKET_SENDBUFFER_SIZE)(double); // size: socket
MAYBEEXTERN SOCKET_SENDBUFFER_SIZE socket_sendbuffer_size;
typedef void  (*SOCKET_SENDBUFFER_LIMIT)(double, double); // void: socket, size
MAYBEEXTERN SOCKET_SENDBUFFER_LIMIT socket_sendbuffer_limit;
typedef double (*SOCKET_RECEIVEBUFFER_SIZE)(double); // size: socket
MAYBEEXTERN SOCKET_RECEIVEBUFFER_SIZE socket_receivebuffer_size;
typedef double (*UDP_SEND)(double, char *, double); // succeededlimit: udpsocket|buffer, hostname/ip, port
MAYBEEXTERN UDP_SEND udp_send;
typedef double (*UDP_RECEIVE)(double); // receivedanything: udpsocket
MAYBEEXTERN UDP_RECEIVE udp_receive;

// buffer manipulation
typedef double (*BUFFER_CREATE)(); // buffer
MAYBEEXTERN BUFFER_CREATE buffer_create;
typedef void   (*BUFFER_DESTROY)(double); // void: buffer
MAYBEEXTERN BUFFER_DESTROY buffer_destroy;
typedef void   (*BUFFER_CLEAR)(double); // void: buffer
MAYBEEXTERN BUFFER_CLEAR buffer_clear;
typedef double (*BUFFER_SIZE)(double); // size: buffer
MAYBEEXTERN BUFFER_SIZE buffer_size;
typedef double (*BUFFER_BYTES_LEFT)(double); // size: buffer|socketReceiveBuf
MAYBEEXTERN BUFFER_BYTES_LEFT buffer_bytes_left;
typedef void   (*BUFFER_SET_READPOS)(double); // void: buffer|socketReceiveBuf
MAYBEEXTERN BUFFER_SET_READPOS buffer_set_readpos;

typedef void   (*WRITE_UBYTE)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_UBYTE write_ubyte;
typedef void   (*WRITE_BYTE)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_BYTE write_byte;
typedef void   (*WRITE_USHORT)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_USHORT write_ushort;
typedef void   (*WRITE_SHORT)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_SHORT write_short;
typedef void   (*WRITE_UINT)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_UINT write_uint;
typedef void   (*WRITE_INT)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_INT write_int;
typedef void   (*WRITE_FLOAT)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_FLOAT write_float;
typedef void   (*WRITE_DOUBLE)(double, double); // void: buffer|SocketSendBuf, real
MAYBEEXTERN WRITE_DOUBLE write_double;
typedef void   (*WRITE_STRING)(double, char *); // void: buffer|SocketSendBuf, string
MAYBEEXTERN WRITE_STRING write_string;
typedef void   (*WRITE_BUFFER)(double, double); // void: target, source
MAYBEEXTERN WRITE_BUFFER write_buffer;
typedef double (*WRITE_BUFFER_PART)(double, double, double); // actualsize: target, source, size
MAYBEEXTERN WRITE_BUFFER_PART write_buffer_part;

typedef double (*READ_UBYTE)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_UBYTE read_ubyte;
typedef double (*READ_BYTE)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_BYTE read_byte;
typedef double (*READ_USHORT)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_USHORT read_ushort;
typedef double (*READ_SHORT)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_SHORT read_short;
typedef double (*READ_UINT)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_UINT read_uint;
typedef double (*READ_INT)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_INT read_int;
typedef double (*READ_FLOAT)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_FLOAT read_float;
typedef double (*READ_DOUBLE)(double); // value: buffer|SocketSendBuf
MAYBEEXTERN READ_DOUBLE read_double;
typedef char * (*READ_STRING)(double, double); // string: buffer|SocketSendBuf, size
MAYBEEXTERN READ_STRING read_string;

// endian
typedef void   (*SET_LITTLE_ENDIAN_GLOBAL)(double); // void: buffer|SocketSendBuf, size
MAYBEEXTERN SET_LITTLE_ENDIAN_GLOBAL set_little_endian_global;
typedef void   (*SET_LITTLE_ENDIAN)(double, double); // void: buffer|SocketSendBuf, size
MAYBEEXTERN SET_LITTLE_ENDIAN set_little_endian;

// hostname
typedef double (*IP_LOOKUP_CREATE)(char *); // lookup: hostname
MAYBEEXTERN IP_LOOKUP_CREATE ip_lookup_create;
typedef double (*IPV4_LOOKUP_CREATE)(char *); // lookup: hostname
MAYBEEXTERN IPV4_LOOKUP_CREATE ipv4_lookup_create;
typedef double (*IPV6_LOOKUP_CREATE)(char *); // lookup: hostname
MAYBEEXTERN IPV6_LOOKUP_CREATE ipv6_lookup_create;

typedef double (*IP_LOOKUP_READY)(double); // isfinished: lookup
MAYBEEXTERN IP_LOOKUP_READY ip_lookup_ready;
typedef double (*IP_LOOKUP_HAS_NEXT)(double); // continues: lookup
MAYBEEXTERN IP_LOOKUP_HAS_NEXT ip_lookup_has_next;
typedef char * (*IP_LOOKUP_NEXT_RESULT)(double); // ip: lookup
MAYBEEXTERN IP_LOOKUP_NEXT_RESULT ip_lookup_next_result;
typedef void   (*IP_LOOKUP_DESTROY)(double); // continues: lookup
MAYBEEXTERN IP_LOOKUP_DESTROY ip_lookup_destroy;

typedef double (*IP_IS_V4)(char *); // whether: ip
MAYBEEXTERN IP_IS_V4 ip_is_v4;
typedef double (*IP_IS_V6)(char *); // whether: ip
MAYBEEXTERN IP_IS_V6 ip_is_v6;

// misc
typedef char * (*SOCKET_REMOTE_IP)(double); // ip: socket
MAYBEEXTERN SOCKET_REMOTE_IP socket_remote_ip;
typedef double (*SOCKET_LOCAL_PORT)(double); // port: socket
MAYBEEXTERN SOCKET_LOCAL_PORT socket_local_port;
typedef double (*SOCKET_REMOTE_PORT)(double); // port: socket
MAYBEEXTERN SOCKET_REMOTE_PORT socket_remote_port;
typedef double (*SOCKET_HAS_ERROR)(double); // haserror: socket|acceptor
MAYBEEXTERN SOCKET_HAS_ERROR socket_has_error;
typedef char * (*SOCKET_ERROR)(double); // error: socket|acceptor
MAYBEEXTERN SOCKET_ERROR socket_error;

int faucnet_init();
void faucnet_exit();

#endif
