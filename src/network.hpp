#include <vector>
#include <string>

namespace Sys
{
    double local_socket;
    struct Message
    {
        long id;
        bool droppable;
        unsigned short message;
        double buffer;
    };
    struct Connection
    {
        Connection ( std::string hostname, int port );
        std::string hostname;
        int port;
        
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
        
        void send ( bool droppable, unsigned subsystem, double buffer );
        void think ();
    };
    Connection::Connection ( std::string hostname, int port )
    : hostname(hostname)
    , port(port)
    , last_droppable_packet(0)
    , last_undroppable_packet(0)
    , acked_undroppable_packet(0)
    , sent_droppable_packet(0)
    , sent_undroppable_packet(0)
    { }
    
    bool assign ( Connection remote, bool droppable, short message, void(*processor)(double) );
    bool bind_reception ( int port );
    
}
