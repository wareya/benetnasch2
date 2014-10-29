#ifndef INCLUDE_NETWORK
#define INCLUDE_NETWORK

#include <vector>
#include <unordered_map>
#include <string>
#include "blib.hpp"

namespace Net
{
    enum {
        MESSAGE_UNDROPPABLE,
        MESSAGE_DROPPABLE,
        ACKNOWLEDGMENT,
        CONNECTION_REQUEST,
        CONNECTION_ACKNOWLEDGED
    };
    extern double local_socket;
    struct Message
    {
        long id;
        bool droppable;
        unsigned short message;
        double buffer;
        double sendtime;
    };
    
    struct Connection
    {
        Connection ( const char * hostname, int port );
        void send_or_resend_connection_request();
        std::string hostname;
        int port;
        double hostname_lookup;
        
        // ids of the last recieved packets from the other machine
        // out-of-order droppable packets are discarded
        // early undroppable packets are discarded
        long last_droppable_packet;
        long last_undroppable_packet;
        
        // last packet that the machine on the other end responded too
        long acked_undroppable_packet;
        
        // incrementors
        long sent_droppable_packet;
        long sent_undroppable_packet;
        
        // connection initializion info (for the side starting the a remote connection)
        bool ready; // whether the remote has acknowledged our connection
        double connection_send_time;
        
        std::vector<Message*> undroppable_send_queue;
    };
    
    extern std::vector<Connection*> connections;
    
    typedef void(*processor)(Connection * /*connection*/, double /*buffer*/ );
    
    extern std::unordered_map<unsigned int, processor> handlers;
    
    bool init ( int port );
    bool think ( ); // handle resending, callbacks, hostname lookups
    void send ( Connection * connection, bool droppable, unsigned short message, double buffer );
    void connect ( Connection * connection );
    int assign ( bool droppable, unsigned short message, processor );
}

#endif // guard
