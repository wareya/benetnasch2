#include "nethandlers.hpp"
#include "clientdata.hpp"

#include "../network.hpp"
#include "../netconst.hpp"

#include "../components/gamecomponents.hpp"
#include "../components/player.hpp"
#include "../serverplayer.hpp"

#include "../blib.hpp"
#include "../bengine.hpp"

namespace Sys
{
    void DisconnectionPseudoCallback(Net::Connection * connection)
    {
        // shut down networking
        Sys::clear_processors();
        
        Net::close();
        
        delete Sys::server;
        Sys::server = nullptr;
        Net::connections.clear();
        
        // shut down client state
        
        Sys::myself = nullptr;
        Sys::speeds.clear();
        Sys::did_send_playerrequest = false;
        
        Sys::myinput.myplayerinput.clearInput(); // clear inputs
        Sys::myinput.myplayerinput.cycleInput(); // clear history
        
        Sys::Players.killall();
        Sys::Bullets.killall();
        
        Sys::PlayerList::Clear();
    }

    void process_message_removeplayer(Net::Connection * connection, double buffer)
    {
        auto pid = read_ubyte(buffer);
        if(pid == (playerid)(-1))
            return;
        std::cout << "removing player " << pid << "\n";
        auto player = PlayerList::Slots[pid]->player;
        delete player;
        Sys::PlayerList::Remove(pid);
    }

    void process_message_addplayer(Net::Connection * connection, double buffer)
    {
        auto len = read_ubyte(buffer);
        auto name = read_string(buffer, len);
        std::cout << "spawning new player " << name << "\n";
        auto player = new Sys::Player(Ent::New(), name);
        
        auto x = read_ushort(buffer);
        auto y = read_ushort(buffer);
        
        player->spawn(x, y);
        
        Sys::PlayerList::AddPlayer(nullptr, player);
        std::cout << "done with " << name << "\n";
    }

    void process_message_playerpositions(Net::Connection * connection, double buffer)
    {
        auto numbytes = 12;
        for(auto i = 0; i < buffer_size(buffer); i += numbytes)
        {
            auto pix = read_ubyte(buffer);
            Sys::ServerPlayer * serverplayer = NULL;
            if(pix < Sys::PlayerList::Slots.size())
                serverplayer = Sys::PlayerList::Slots[pix];
            #ifdef B_NET_DEBUG_PRINTPACK
                std::cout << "p pix: " << pix << " out of " << Sys::PlayerList::Slots.size() << "\n";
            #endif
            if(serverplayer)
            {
                auto input = &serverplayer->player->input;
                auto character = serverplayer->player->character;
                if(character)
                {
                    character->position->x = read_ushort(buffer)/10;
                    character->position->y = read_ushort(buffer)/10;
                    character->hspeed = read_byte(buffer)/5;
                    character->vspeed = read_byte(buffer)/5;
                    input->setInputsAsBitfield( read_ushort(buffer) );
                    input->aimDirection = read_ushort(buffer)*360.0/0x10000;
                    input->aimDistance = read_ubyte(buffer)*2;
                }
                else for (auto i = numbytes; i > 0; --i)
                    read_ubyte(buffer);
            }
            // somehow we got more inputs than there are players, or have an invalid
            // serverplayer, but whatever. avoid corrupting the rest of the stream.
            else for (auto i = numbytes; i > 0; --i)
                read_ubyte(buffer);
        }
    }

    void process_message_playerinputs(Net::Connection * connection, double buffer)
    {
        auto numbytes = 6;
        for(auto i = 0; i < buffer_size(buffer); i += numbytes)
        {
            auto pix = read_ubyte(buffer);
            Sys::ServerPlayer * serverplayer = NULL;
            if(pix < Sys::PlayerList::Slots.size())
                serverplayer = Sys::PlayerList::Slots[pix];
            #ifdef B_NET_DEBUG_PRINTPACK
                std::cout << "i pix: " << pix << " out of " << Sys::PlayerList::Slots.size() << "\n";
            #endif
            if(serverplayer)
            {
                auto input = &serverplayer->player->input;
                auto character = serverplayer->player->character;
                if(character)
                {
                    input->setInputsAsBitfield( read_ushort(buffer) );
                    input->aimDirection = read_ushort(buffer)*360.0/0x10000;
                    input->aimDistance = read_ubyte(buffer)*2;
                }
                else for (auto i = numbytes; i > 0; --i)
                    read_ubyte(buffer);
            }
            // somehow we got more inputs than there are players, or have an invalid
            // serverplayer, but whatever. avoid corrupting the rest of the stream.
            else for (auto i = numbytes; i > 0; --i)
                read_ubyte(buffer);
        }
    }

    void process_message_serveplayer(Net::Connection * connection, double buffer)
    {
        std::cout << "getting a playerlist!\n";
        while(buffer_bytes_left(buffer))
        {
            auto len = read_ubyte(buffer);
            auto name = read_string(buffer, len);
            auto player = new Sys::Player(Ent::New(), name);
            Sys::PlayerList::AddPlayer(nullptr, player);
            
            if(read_ubyte(buffer)) // has character
            {
                auto x = read_ushort(buffer)/10;
                auto y = read_ushort(buffer)/10;
                player->spawn(x, y);
            }
            
            if(buffer_bytes_left(buffer) == 0)
            {
                Sys::myself = player;
                if(player->character)
                    player->character->myself = true;
            }
        }
        std::cout << "got a playerlist\n";
    }

    void add_processors()
    {
        assign ( 1, SERVERMESSAGE::PLAYERINPUTS, &process_message_playerinputs );
        assign ( 0, SERVERMESSAGE::ADDPLAYER, &process_message_addplayer );
        assign ( 0, SERVERMESSAGE::REMOVEPLAYER, &process_message_removeplayer );
        assign ( 1, SERVERMESSAGE::PLAYERPOSITIONS, &process_message_playerpositions );
        assign ( 0, SERVERMESSAGE::SERVEPLAYER, &process_message_serveplayer );
    }
    void clear_processors()
    {
        Net::handlers.clear();
    }
}
