#include "network.hpp"
#include "blib.hpp"
#include <string>

#define B_NET_DEBUG_PRINTPACK

#ifdef B_NET_DEBUG_PRINTPACK
#include <iostream>
#endif

namespace Net
{
    double local_socket;
    std::vector<Connection*> connections;
    std::unordered_map<unsigned int, processor> handlers;
    
    Connection::Connection ( const char * hostname, int port )
    : hostname((char *)(hostname))
    , port(port)
    , last_droppable_packet(0)
    , last_undroppable_packet(0)
    , acked_undroppable_packet(0)
    , sent_droppable_packet(0)
    , sent_undroppable_packet(0)
    , ready(false)
    , connection_send_time(0)
    {
        hostname_lookup = ip_lookup_create((char *)hostname);
    }
    
    void Connection::send_or_resend_connection_request()
    {
        if(ready)
        {
            #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "Remote " << hostname << ":" << port << " is ready.\n";
            #endif
            return;
        }
        else
        {
            #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "Sending a connection request to " << hostname << ":" << port << "\n";
            #endif
            udp_send(local_socket, "127.0.0.1", 9); // discard socket buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
            write_ubyte(local_socket, CONNECTION_REQUEST);
            udp_send(local_socket, (char *)hostname.c_str(), port);
            connection_send_time = Time::get_us();
        }
    }
    
    bool init ( int port )
    {
        #ifdef B_NET_DEBUG_PRINTPACK
        std::cout << "Initing local socket on " << port << "\n";
        #endif
        local_socket = udp_bind(port);
        return 0;
    }
    
    bool think ( )
    {
        for(auto connection : connections)
        {
            // update hostname lookups
            if(connection->hostname_lookup > 0) // hostname lookup is running or recently finished
            {
                if(ip_lookup_ready(connection->hostname_lookup)) // hostname lookup finished
                {
                    if(ip_lookup_has_next(connection->hostname_lookup)) // finished with valid IP
                        connection->hostname = ip_lookup_next_result(connection->hostname_lookup);
                    
                    #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Doing hostname schenanegans.\n";
                    #endif
                    ip_lookup_destroy(connection->hostname_lookup);
                    connection->hostname_lookup = -1;
                }
            }
            
            for(auto message : connection->undroppable_send_queue)
            {
                if(Time::get_us() - message->sendtime > 1000*1000) // TODO: replace hardcoded 200 ms with ping*1.5 or something
                {
                    #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Resending a message to " << connection->hostname << ":" << connection->port << "\n";
                    #endif
                    message->sendtime = Time::get_us();
                    udp_send(message->buffer, (char *)connection->hostname.c_str(), connection->port);
                }
            }
            
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
        while(udp_receive(local_socket))
        {
            auto remote_ip = socket_remote_ip(local_socket);
            auto remote_port = socket_remote_port(local_socket);
            udp_send(local_socket, "127.0.0.1", 9); // discard outgoing buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
            
            #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "Got a packet from " << remote_ip << ":" << remote_port << "\n";
            #endif
            
            auto type = read_ubyte(local_socket);
            
            // TODO: Move this down so connections can actually be, you know, initiated
            Connection * remote = NULL;
            for (auto connection : connections)
            {
                // TODO: There has really truly honestly GOT to be a much better way to do this
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
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Throwing away packet from "
                              << socket_remote_ip(local_socket) << ":" << socket_remote_port(local_socket)
                              << " that would be of type "
                              << type << "\n";
                #endif
                continue;
            }
            else if (type == CONNECTION_REQUEST)
            {
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Got connection request from "
                              << remote_ip << ":" << remote_port << "\n";
                #endif
                if( remote == NULL ) // make sure only to consider connection requests from people who aren't connected (it can/will happen)
                {
                    #ifdef B_NET_DEBUG_PRINTPACK
                        std::cout << "It's a new connection.\n";
                    #endif
                    auto connection = new Connection(socket_remote_ip(local_socket), socket_remote_port(local_socket));
                    #ifdef B_NET_DEBUG_PRINTPACK
                        std::cout << "Stored as: " << connection->hostname << ":" << connection->port << "\n";
                    #endif
                    connections.push_back(connection);
                    ip_lookup_destroy(connection->hostname_lookup); // remote messages should already have known IPs
                    connection->hostname_lookup = -1;
                    connection->ready = true;
                }
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Sending connection acknowledgment.\n";
                #endif
                write_ubyte(local_socket, CONNECTION_ACKNOWLEDGED);
                udp_send(local_socket, remote_ip, remote_port);
            }
            
            switch(int(type))
            {
            case MESSAGE_UNDROPPABLE:
            case MESSAGE_DROPPABLE:
            {
                auto id = read_uint(local_socket);
                
                #ifdef B_NET_DEBUG_PRINTPACK
                    std::cout << "Got a (droppable: " << type << ") message from "
                              << socket_remote_ip(local_socket) << ":" << socket_remote_port(local_socket)
                              << " headed " << id << "\n";
                #endif
                
                // ACK undroppable packets immediately
                if(type == MESSAGE_UNDROPPABLE)
                {
                    if(id == remote->last_undroppable_packet+1)
                    {
                        write_ubyte(local_socket, 3);
                        write_uint(local_socket, id);
                        udp_send(local_socket, socket_remote_ip(local_socket), socket_remote_port(local_socket));
                        remote->last_undroppable_packet = id;
                    }
                    else
                        break; // out of order (late) or previous dropped
                        // TODO FUTURE: Cache previous dropped messages so that catching up is faster
                }
                if(type == MESSAGE_DROPPABLE)
                {
                    if(id > remote->last_droppable_packet)
                        remote->last_undroppable_packet = id;
                    else
                        break; // out of order (late)
                }
                
                auto message = read_ushort(local_socket);
                message = unsigned(message) & (unsigned(type) << (sizeof(unsigned short)*8));
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
            case ACKNOWLEDGMENT:
            {
                auto id = read_uint(local_socket);
                if( id > remote->acked_undroppable_packet )
                    remote->acked_undroppable_packet = id;
                
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
    
    
    void send ( Connection * connection, bool droppable, unsigned short message, double buffer )
    {
        #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "Check stored IP/port: " << connection->hostname << ":" << connection->port << "\n";
            std::cout << "outer ptr: " << connection << "\n";
            std::cout << "inner ptr: " << &connection->hostname << "\n";
        #endif

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
        
        udp_send(local_socket, "127.0.0.1", 9); // discard socket buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
        write_buffer(local_socket, temp);
        udp_send(local_socket, connection->hostname.c_str(), connection->port);
        
        #ifdef B_NET_DEBUG_PRINTPACK
            std::cout << "Sent a message to " << connection->hostname << ":" << connection->port << "\n";
            std::cout << "outer ptr: " << connection << "\n";
            std::cout << "inner ptr: " << &connection->hostname << "\n";
        #endif
        
        if(!droppable) // we want to store our send buffer (rather than the raw buffer, for simplicity's sake) for resending
            connection->undroppable_send_queue.push_back(new Message ({connection->sent_undroppable_packet-1, droppable, message, temp, Time::get_us()}));
        else // think handles clean of undroppables, but we can trash the droppables here
            buffer_destroy(temp);
    }
    
    
    int assign ( bool droppable, unsigned short message, processor processor )
    {
        #ifdef B_NET_DEBUG_PRINTPACK
        std::cout << "Trying to bind a processor to " << message << "\n";
        #endif
        unsigned int key = (droppable << (sizeof(unsigned short)*8)) & message;
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
