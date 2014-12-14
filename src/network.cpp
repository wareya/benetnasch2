#include "network.hpp"
#include "blib.hpp"
#include <string>

#if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
#include <iostream>
#endif

#ifdef B_NET_DEBUG_PRINTPACK
#define B_NET_DEBUG_MISTAKES
#endif

void trash_buffer(double socket) // discard socket buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
{
    // own function for easy removal in the future
    udp_send(socket, "127.0.0.1", 9);
}

// wrapper in case things need to be logged universally
double udp_send_wrapper(double withandfrom, const char * hostname, double port)
{
    auto x = udp_send(withandfrom, hostname, port);
    #ifdef B_NET_DEBUG_PRINTPACK
    std::cout << "udp_send returned " << x << "\n";
    #endif
    return x;
}

namespace Net
{
    double local_socket;
    // list of external connections
    std::vector<Connection*> connections;
    // assignment map of message id to function pointer which processes messages with that id
    std::unordered_map<unsigned int, processor> handlers;
    
    Connection::Connection ( const char * hostname, int port )
    : hostname((char *)(hostname))
    , port(port)
    , last_droppable_packet(-1)
    , last_undroppable_packet(-1)
    , acked_undroppable_packet(-1)
    , sent_droppable_packet(0)
    , sent_undroppable_packet(0)
    , ready(false)
    , connection_send_time(0)
    {
        hostname_lookup = ip_lookup_create(hostname);
    }
    
    // if relevant, send a connection request to the desired remote host (client code)
    void Connection::send_or_resend_connection_request()
    {
        if(ready)
        {
            #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
            std::cout << "Remote " << hostname << ":" << port << " is ready.\n";
            #endif
            return;
        }
        else
        {
            #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
            std::cout << "Sending a connection request to " << hostname << ":" << port << "\n";
            #endif
            trash_buffer(local_socket);
            write_ubyte(local_socket, CONNECTION_REQUEST);
            udp_send_wrapper(local_socket, hostname.c_str(), port);
            connection_send_time = Time::get_us();
        }
    }
    std::string Connection::as_string()
    {
        return hostname + ":" + std::to_string(port);
    }
    
    // create local socket listening on given port
    bool init ( int port )
    {
        #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
        std::cout << "Initing local socket on " << port << "\n";
        #endif
        local_socket = udp_bind(port);
        return 0;
    }
    
    // shuffle internal state as if handling events
    bool think ( )
    {
        trash_buffer(local_socket);
        // things regarding remote connections...
        for(auto connection : connections)
        {
            // update hostname lookups (typically just for the server)
            if(connection->hostname_lookup > 0) // hostname lookup is running or recently finished
            {
                if(ip_lookup_ready(connection->hostname_lookup)) // hostname lookup finished
                {
                    if(ip_lookup_has_next(connection->hostname_lookup)) // finished with valid IP
                        connection->hostname = ip_lookup_next_result(connection->hostname_lookup);
                    
                    #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
                    std::cout << "Doing hostname schenanegans.\n";
                    #endif
                    ip_lookup_destroy(connection->hostname_lookup);
                    connection->hostname_lookup = -1;
                }
            }
            // check for probably-dropped messages and resend them
            for(auto message : connection->undroppable_send_queue)
            {
                if(Time::get_us() - message->sendtime > 1000*200) // TODO: replace hardcoded 200 ms with ping*1.5 or something
                {
                    #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Resending a message to " << connection->hostname.c_str() << ":" << connection->port << "\n";
                    #endif
                    message->sendtime = Time::get_us();
                    write_buffer(local_socket, message->buffer);
                    udp_send_wrapper(local_socket, connection->hostname.c_str(), connection->port);
                    std::cout << "resend " << read_hex(message->buffer, buffer_size(message->buffer)) << "\n";
                }
            }
            // resend connection request if we haven't heard back for the first time yet
            if(!connection->ready)
            {
                if (Time::get_us() - connection->connection_send_time > 1000*1000) // resend every second until we get an ack
                    connection->send_or_resend_connection_request();
            }
        }
        /*  Packet format
            ubyte type:
            droppable|undroppable (message)
            {
                long id
                ushort message_enumeration
                remainder message
            }
            acknowledgment
            {
                long last_accepted_undroppable
            }
            connection/acknowledgement
            {}
        */
        
        // Read out received messages
        while(udp_receive(local_socket))
        {
            buffer_set_readpos(local_socket, 0);
            auto remote_ip = socket_remote_ip(local_socket);
            auto remote_port = socket_remote_port(local_socket);
            
            #ifdef B_NET_DEBUG_PRINTPACK
                std::cout << "Got a packet from " << remote_ip << ":" << remote_port << "\n";
            #endif
            
            auto type = read_ubyte(local_socket);
            
            Connection * remote = NULL;
            // TODO: There has really truly honestly GOT to be a much better way to do this
            // find connection (if any) associated with message source
            for (auto connection : connections)
            {
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Comparing connection hostname '" << connection->hostname << "' to ip '" << remote_ip << "'\n";
                    std::cout << "Comparing connection port '" << connection->port << "' to port '" << remote_port << "'\n";
                #endif
                if (std::string(connection->hostname) == remote_ip and connection->port == remote_port)
                {
                    remote = connection;
                    break;
                }
            }
            // Ignore message if from unknown source and not a request to connect
            // This is only sent by the initiating side. The initiating side should
            // make a Connection for its remote manually, instead of from a response.
            if(remote == NULL and type != CONNECTION_REQUEST)
            {
                #ifdef B_NET_DEBUG_MISTAKES
                    std::cout << "Throwing away packet from "
                              << remote_ip << ":" << remote_port
                              << " that would be of type "
                              << type << "\n";
                #endif
                continue;
            }
            else if (type == CONNECTION_REQUEST)
            {
                #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
                    std::cout << "Got connection request from "
                              << remote_ip << ":" << remote_port << "\n";
                #endif
                if( remote == NULL ) // make sure only to consider connection requests from people who aren't connected (it can/will happen)
                {
                    #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
                        std::cout << "It's a new connection.\n";
                    #endif
                    auto connection = new Connection(remote_ip, remote_port);
                    #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
                        std::cout << "Stored as: " << connection->hostname << ":" << connection->port << "\n";
                    #endif
                    connections.push_back(connection);
                    ip_lookup_destroy(connection->hostname_lookup); // remote hostname is already an IP
                    connection->hostname_lookup = -1;
                    connection->ready = true;
                }
                else
                {
                    #if defined(B_NET_DEBUG_MISTAKES) || defined(B_NET_DEBUG_CONNECTION)
                        std::cout << "Ignoring reconnection from " << remote_ip << "\n";
                    #endif
                }
                #if defined(B_NET_DEBUG_PRINTPACK) || defined(B_NET_DEBUG_CONNECTION)
                    std::cout << "Sending connection acknowledgment.\n";
                #endif
                write_ubyte(local_socket, CONNECTION_ACKNOWLEDGED);
                udp_send_wrapper(local_socket, remote_ip, remote_port);
            }
            
            // if we've fallen through to here it's a "message"
            switch(int(type))
            {
            // high-level messages
            case MESSAGE_UNDROPPABLE:
            case MESSAGE_DROPPABLE:
            {
                auto id = read_uint(local_socket);
                auto message = read_ushort(local_socket);
                
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Got a (droppable: " << type << ") message from "
                              << remote_ip << ":" << remote_port
                              << " ided " << id 
                              << " messaged " << message << "\n";
                #endif
                
                // ACK undroppable packets immediately
                if(type == MESSAGE_UNDROPPABLE)
                {
                    if(id == remote->last_undroppable_packet+1)
                    {
                        #ifdef B_NET_DEBUG_PRINTPACK
                            puts("Acking message");
                        #endif
                        write_ubyte(local_socket, ACKNOWLEDGMENT);
                        write_uint(local_socket, id);
                        udp_send_wrapper(local_socket, remote_ip, remote_port);
                        remote->last_undroppable_packet = id;
                    }
                    else
                    {
                        #ifdef B_NET_DEBUG_MISTAKES
                            std::cout << "Ignoring message " << id << " vs " << remote->last_undroppable_packet << "\n";
                        #endif
                        break; // duplicate or dependent on late/dropped message
                    }
                    // TODO FUTURE: Cache packets ignored for being before their predecessors so that catching up is faster
                }
                if(type == MESSAGE_DROPPABLE)
                {
                    if(id > remote->last_droppable_packet)
                        remote->last_droppable_packet = id;
                    else
                    {
                        #ifdef B_NET_DEBUG_MISTAKES
                            puts("Ignoring message (out-of-order)");
                        #endif
                        break; // out of order (late)
                    }
                }
                // convert message and droppable to handler ID TODO: pure function
                message = unsigned(message) | (unsigned(type) << (sizeof(unsigned short)*8));
                if(handlers.find(message) != handlers.end())
                {
                    if(handlers[message] != NULL)
                    {
                        auto bulk = buffer_create();
                        write_buffer_part(bulk, local_socket, socket_receivebuffer_size(local_socket));
                        handlers[message](remote, bulk);
                        buffer_destroy(bulk);
                    }
                }
                break;
            }
            // acknowledgment of high-level message
            case ACKNOWLEDGMENT:
            {
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Got an ack!\n";
                #endif
                auto id = read_uint(local_socket);
                if ( id > remote->acked_undroppable_packet )
                    remote->acked_undroppable_packet = id;
                std::vector<Message*> marked_for_removal;
                for ( auto message : remote->undroppable_send_queue )
                {
                    if ( id <= remote->acked_undroppable_packet )
                        marked_for_removal.push_back(message);
                }
                for ( auto message : marked_for_removal )
                {
                    unsigned i;
                    for (i = 0; remote->undroppable_send_queue[i] != message and i < remote->undroppable_send_queue.size(); ++i) ;
                    if(i == remote->undroppable_send_queue.size())
                        std::cout << "Bad message removal in ack!";
                    else
                    {
                        remote->undroppable_send_queue.erase(remote->undroppable_send_queue.begin()+i);
                        delete message;
                    }
                }
                break;
            }
            case CONNECTION_ACKNOWLEDGED:
                remote->ready = true;
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << remote_ip << ":" << remote_port << "-senpai acknowledged me!\n";
                    std::cout << "Check stored IP/port: " << remote->hostname << ":" << remote->port << "\n";
                #endif
            }
        }
        return 0;
    }
    
    // high-level function to send a message according to parameters
    void send ( Connection * connection, bool droppable, unsigned short message, double buffer )
    {
        // blank buffer for porpoises
        double temp = buffer_create();
        // write up header etc to temp buffer
        write_ubyte(temp, droppable);
        if(droppable)
        {
            write_uint(temp, connection->sent_droppable_packet);
            connection->sent_droppable_packet++;
        }
        else
        {
            write_uint(temp, connection->sent_undroppable_packet);
            connection->sent_undroppable_packet++;
        }
        write_ushort(temp, message);
        write_buffer(temp, buffer);
        
        trash_buffer(local_socket);
        
        write_buffer(local_socket, temp);
        
        #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "About to send " << buffer_size(temp) << " bytes with header " << message << ".\n";
        #endif
        
        if(connection->ready)
            udp_send_wrapper(local_socket, connection->hostname.c_str(), connection->port);
        
        #ifdef B_NET_DEBUG_PRINTPACK
            else
                std::cout << "Connection not ready in send().\n";
            std::cout << "Sent a message to " << connection->hostname.c_str() << ":" << connection->port << "\n";
        #endif
            
        if(!droppable) // we want to store our send buffer (rather than the raw buffer, for simplicity's sake) for resending
        {
            connection->undroppable_send_queue.push_back(new Message ({connection->sent_undroppable_packet-1, droppable, message, temp, Time::get_us()}));
            buffer_set_readpos(temp, 0);
        }
        else // think handles clean of undroppables, but we can trash the droppables here
        {
            buffer_destroy(temp);
        }
    }
    
    
    int assign ( bool droppable, unsigned short message, processor processor )
    {
        unsigned int key = (droppable << (sizeof(unsigned short)*8)) | message;
        #ifdef B_NET_DEBUG_PRINTPACK
        std::cout << "Trying to bind a processor to " << message << "\n";
        std::cout << "Key: " << key << "\n";
        #endif
        // Possible conditions for assignment:
        // - Handle already taken by null function
        // - Handle not taken
        // - Null function being passed (check)
        if(handlers.find(key) == handlers.end() or processor == NULL) // handle not taken
            goto valid;
        if (handlers.find(key) != handlers.end()) // replacing null
            if (handlers[key] == NULL)
                goto valid;
        goto invalid;
        valid:
        {
            handlers[key] = processor;
            return 0;
        }
        invalid:
        if (handlers.find(key) != handlers.end() and processor != NULL)
            return -1; // existing processor
        else
            return -2; // Unknown error, possibly mid-execution state change
    }
}
