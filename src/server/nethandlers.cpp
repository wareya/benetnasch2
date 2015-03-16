#include "nethandlers.hpp"

#include "../network.hpp"
#include "../netconst.hpp"

#include "../serverplayer.hpp"
#include "../maps.hpp"

#include "../blib.hpp"
#include "../bengine.hpp"

namespace Sys
{
    void DisconnectionPseudoCallback(Net::Connection * connection)
    {
        auto serverplayer = PlayerList::IdFromConnection(connection);
        if(serverplayer == (playerid)(-1))
            return;
        auto player = PlayerList::Slots[serverplayer]->player;
        delete player;
        PlayerList::Remove(serverplayer);
        
        auto mirror = buffer_create();
        write_ubyte(mirror, serverplayer);
        for(auto serverplayer : PlayerList::Slots)
            Net::send(serverplayer->connection, 0, SERVERMESSAGE::REMOVEPLAYER, mirror);
        buffer_destroy(mirror);
    }
    
    void process_message_input(Net::Connection * connection, double buffer)
    {
        auto serverplayer = Sys::PlayerList::FromConnection(connection);
        if(serverplayer != NULL)
        {
            Input::PlayerInput & input = serverplayer->player->input;
            
            input.netkeys = read_ushort(buffer);
            input.netaimdir = read_ushort(buffer);
            input.netaimdist = read_ubyte(buffer);
            
            input.setInputsAsBitfield ( input.netkeys ) ;
            input.aimDirection = input.netaimdir*360.0/0x10000;
            input.aimDistance = input.netaimdist*2;
        }
    }

    double build_message_serveplayer(Sys::ServerPlayer * targetplayer, double buffer)
    {
        for(auto serverplayer : Sys::PlayerList::Slots)
        {
            write_ubyte(buffer, serverplayer->player->name.size());
            std::string asdf = serverplayer->player->name;
            asdf.resize(255, '\0');
            write_string(buffer, asdf.data());
            if(serverplayer->player->character)
            {
                write_ubyte(buffer, 1);
                write_ushort(buffer, serverplayer->player->character->position->x*10);
                write_ushort(buffer, serverplayer->player->character->position->y*10);
            }
            else
                write_ubyte(buffer, 0);
        }
        return buffer;
    }

    void process_message_playerrequest(Net::Connection * connection, double buffer)
    {
        if(Sys::PlayerList::FromConnection(connection) == NULL)
        {
            auto namelen = read_ubyte(buffer);
            auto name = read_string(buffer, namelen);
            auto player = new Sys::Player(Ent::New(), name);
            unsigned playerslot = Sys::PlayerList::AddPlayer(connection, player);
            auto serverplayer = Sys::PlayerList::Slots[playerslot];
            
            player->spawn(Maps::width/2, Maps::height/2);
            
            auto response = buffer_create();
            write_ubyte(response, namelen);
            write_string(response, name);
            write_ushort(response, Maps::width/2);
            write_ushort(response, Maps::height/2);
            
            // tell other players about the new player
            for ( unsigned i = 0; i+1/*last slot is joining player*/ < Sys::PlayerList::Slots.size(); ++i )
            {
                auto current = Sys::PlayerList::Slots[i]->connection;
                std::cout << "Sending player " << playerslot << " to " << i << " (" << current->as_string() << ")\n";
                Net::send(current, 0, SERVERMESSAGE::ADDPLAYER, response);
            }
            
            buffer_clear(response);
            
            // tell the new player about everyone, including theirself
            build_message_serveplayer(serverplayer, response);
            Net::send(connection, 0, SERVERMESSAGE::SERVEPLAYER, response);
            buffer_destroy(response);
        }
    }
    
    void add_processors()
    {
        Net::assign ( 1, CLIENTMESSAGE::INPUT, &process_message_input );
        Net::assign ( 0, CLIENTMESSAGE::PLAYERREQUEST, &process_message_playerrequest );
    }
    void clear_processors()
    {
        Net::handlers.clear();
    }
}
