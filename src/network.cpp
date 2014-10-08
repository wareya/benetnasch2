#include "network.hpp"
#include "blib.hpp"
#include <string>

namespace Net
{
    bool init ( int port )
    {
        local_socket = port;
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
                    
                    ip_lookup_destroy(connection->hostname_lookup);
                    connection->hostname_lookup = -1;
                }
            }
            
            for(auto message : connection->undroppable_send_queue)
            {
                if(Time::get_us() - message.sendtime > 1000*200) // TODO: replace hardcoded 200 ms with ping*1.5 or something
                {
                    udp_send(message.buffer, connection->hostname, connection->port);
                    message.sendtime = Time::get_us();
                }
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
        */
        while(udp_receive(local_socket))
        {
            udp_send(local_socket, "127.0.0.1", 9); // discard socket buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
            auto full_length = socket_receivebuffer_size(local_socket);
            auto remote_ip = socket_remote_ip(local_socket);
            auto remote_port = socket_remote_port(local_socket);
            
            // TODO: Move this down so connections can actually be, you know, initiated
            Connection * remote = NULL;
            for (auto connection : connections)
            {
                // TODO: There has really truly honestly GOT to be a much better way to do this
                if (std::string(connection->hostname).compare(remote_ip) and connection->port == remote_port)
                {
                    remote = connection;
                    break;
                }
            }
            if(remote == NULL)
                *(char *)0 = 0;
            
            auto type = read_ubyte(local_socket);
            
            switch(int(type))
            {
            case MESSAGE_UNDROPPABLE:
            case MESSAGE_DROPPABLE:
            {
                auto id = read_uint(local_socket);
                
                // ACK immediately
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
                        break; // out of order or previous dropped
                }
                if(type == MESSAGE_DROPPABLE)
                {
                    if(id > remote->last_droppable_packet)
                        remote->last_undroppable_packet = id;
                    else
                        break; // out of order
                }
                
                auto message = read_ushort(local_socket);
                if(handlers.find(message) != handlers.end())
                {
                    auto bulk = buffer_create();
                    write_buffer_part(bulk, local_socket, socket_receivebuffer_size(local_socket));
                    handlers[message](remote, bulk);
                    buffer_destroy(bulk);
                }
                break;
            }
            case ACKNOWLEDGMENT:
                if(read_uint(local_socket) == remote->acked_undroppable_packet+1)
                    remote->acked_undroppable_packet += 1;
                break;
            }
        }
        return 0;
    }
    
    
    void send ( Connection * connection, bool droppable, unsigned short message, double buffer )
    {
        double temp = buffer_create();
        // write up header etc to temp buffer
        write_ubyte(temp, droppable);
        write_uint(temp, droppable?connection->sent_droppable_packet++:connection->sent_undroppable_packet++); // because fuck you that's why
        write_ushort(temp, message);
        write_buffer(temp, buffer);
        
        udp_send(local_socket, "127.0.0.1", 9); // discard socket buffer by sending to port 9 TODO: VERIFY (duplicated TODO)
        write_buffer(local_socket, temp);
        udp_send(local_socket, connection->hostname, connection->port);
        
        if(!droppable) // we want to store our send buffer (rather than the raw buffer, for simplicity's sake) for resending
            connection->undroppable_send_queue.push_back({connection->sent_undroppable_packet-1, droppable, message, temp, Time::get_us()});
        else // think handles clean of undroppables, but we can trash the droppables here
            buffer_destroy(temp);
    }
    
    
    int assign ( bool droppable, unsigned short message, processor processor )
    {
        unsigned int key = (droppable << (sizeof(unsigned short)*8)) & message;
        if(handlers.find(key) == handlers.end()) // none
        {
            handlers[key] = processor;
            return 0;
        }
        else
            return -1;
    }
}
