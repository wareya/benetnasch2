#include <vector>
#include <unordered_map>
#include <string>
#include "blib.hpp"

namespace Net
{
    enum {
        MESSAGE_UNDROPPABLE,
        MESSAGE_DROPPABLE,
        ACKNOWLEDGMENT
    };
    double local_socket;
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
        char * hostname;
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
        
        std::vector<Message> undroppable_send_queue;
    };
    Connection::Connection ( const char *  hostname, int port )
    : hostname((char *)(hostname))
    , port(port)
    , last_droppable_packet(0)
    , last_undroppable_packet(0)
    , acked_undroppable_packet(0)
    , sent_droppable_packet(0)
    , sent_undroppable_packet(0)
    {
        hostname_lookup = ip_lookup_create((char *)hostname);
    }
    
    std::vector<Connection*> connections;
    
    typedef void(*processor)(Connection * /*connection*/, double /*buffer*/ );
    
    std::unordered_map<unsigned int, processor> handlers;
    
    bool init ( int port );
    bool think ( ); // handle resending, callbacks, hostname lookups
    void send ( Connection * connection, bool droppable, unsigned short subsystem, double buffer );
    int assign ( bool droppable, unsigned short message, processor );
}
