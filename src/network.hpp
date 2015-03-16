#ifndef INCLUDE_NETWORK
#define INCLUDE_NETWORK

#include <vector>
#include <unordered_map>
#include <string>
#include "blib.hpp"

namespace Net
{
    enum { // low-level networking header enumerations
        MESSAGE_UNDROPPABLE,
        MESSAGE_DROPPABLE,
        ACKNOWLEDGMENT,
        CONNECTION_REQUEST,
        CONNECTION_ACKNOWLEDGED
    };
    extern double local_socket; // socket with which to receive/send datagrams
    struct Message // Type containing information representing a high-level message
    {
        long id; // chronological sequence indicator
        bool droppable; // whether this message should use the droppable or the undroppable system
        unsigned short message; // type of message (unique between whether droppable)
        double buffer; // faucnet buffer reference containing information
        double sendtime; // time at which the message was sent
    };
    
    struct Connection
    {
        Connection ( const char * hostname, int port );
        void send_or_resend_connection_request();
        std::string hostname;
        int port;
        double hostname_lookup;
        // wait for hostname lookup before initiating an outgoing connection
        bool wait_for_hostname;
        
        std::string as_string();
        
        // ids of the last recieved packets from the other machine
        // out-of-order droppable packets are discarded
        // early undroppable packets are discarded
        long last_droppable_packet;
        long last_undroppable_packet;
        
        // last time we heard back from the other end of the connection
        double last_time;
        
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
    void close ( );
    bool think ( ); // handle resending, callbacks, hostname lookups
    void send ( Connection * connection, bool droppable, unsigned short message, double buffer );
    void connect ( Connection * connection );
    int assign ( bool droppable, unsigned short message, processor );
}

namespace Sys
{
    void DisconnectionPseudoCallback(Net::Connection * connection);
}

#endif // guard
