#include "bengine.hpp"
#include "blib.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "physics.hpp"
#include "input.hpp"
#include "network.hpp"
#include "netconst.hpp"
#include "serverplayer.hpp"
#include "components/player.hpp"
#include "server/think.hpp"

bool sys_init()
{
    Sys::lastQuickUpdate = Time::get_us();
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
        Sys::tems.push_back(&Net::think);
        Sys::tems.push_back(&Sys::Physics); // physics
        Sys::tems.push_back(&Sys::ServerThink);
    #endif
    
    return 1;
}

void process_message_input(Net::Connection * connection, double buffer)
{
    auto r = Sys::ServerPlayers::FromConnection(connection);
    if(r != NULL)
    {
        auto player = r->player;
        
        auto netkeys = read_ushort(buffer);
        auto netaimdir = read_ushort(buffer);
        auto netaimdist = read_ubyte(buffer);
        player->input.setInputsAsBitfield ( netkeys ) ;
        player->input.aimDirection = netaimdir*360.0/0x10000;
        player->input.aimDistance = netaimdist*2;
        
        auto response = buffer_create();
        write_ushort(response, netkeys); // key
        write_ushort(response, netaimdir); // angle
        write_ubyte(response, netaimdist); // distance
        Net::send(connection, 1, SERVERMESSAGE::PLAYERINPUT, response);
        buffer_destroy(response);
    }
}

double build_message_playerlist(Sys::ServerPlayer * targetplayer, double buffer)
{
    write_ubyte(buffer, targetplayer->id);
    for(auto serverplayer : Sys::ServerPlayers::ServerPlayers)
    {
        write_ubyte(buffer, serverplayer->id);
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
    if(Sys::ServerPlayers::FromConnection(connection) == NULL)
    {
        auto namelen = read_ubyte(buffer);
        auto name = read_string(buffer, namelen);
        auto player = new Sys::Player(Ent::New(), name);
        auto serverplayer = Sys::ServerPlayers::Add(connection, player);
        
        player->spawn(Maps::width/2, Maps::height/2);
        
        auto response = buffer_create();
        write_ubyte(response, serverplayer->id);
        write_ubyte(response, namelen);
        write_string(response, name);
        write_ushort(response, Maps::width/2);
        write_ushort(response, Maps::height/2);
        write_ubyte(response, 0);
        
        // tell other players about the new player
        for(auto remote : Sys::ServerPlayers::ServerPlayers)
        {
            if(remote != serverplayer)
                Net::send(serverplayer->connection, 0, SERVERMESSAGE::SPAWNNEWPLAYER, response);
        }
        buffer_clear(response);
        
        // tell the new player about everyone, including theirself
        build_message_playerlist(serverplayer, response);
        Net::send(connection, 0, SERVERMESSAGE::PLAYERLIST, response);
        buffer_clear(response);
    }
}

bool main_init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    SDL_PumpEvents();
    
    if(faucnet_init() < 0)
        return 0;
    
    Net::init(4192);
    
    Net::assign ( 0, CLIENTMESSAGE::INPUT, &process_message_input );
    Net::assign ( 0, CLIENTMESSAGE::PLAYERREQUEST, &process_message_playerrequest );
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
