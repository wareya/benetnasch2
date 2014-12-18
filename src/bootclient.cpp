#include "blib.hpp"
#include "bengine.hpp"
#include "physics.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "components/player.hpp"
#include "input.hpp"
#include "network.hpp"
#include "netconst.hpp"
#include "client/clientdata.hpp"
#include "client/think.hpp"
#include "physics.hpp"
#include "serverplayer.hpp"
#include <lua/lua.hpp>

lua_State * LuaHUD;
bool do_hud()
{
    lua_pushvalue(LuaHUD, -1);
    lua_pcall(LuaHUD, 0, 0, 0);
    return 0;
}

int tolua_print (lua_State * L)
{
    std::cout << lua_tostring(L, 1) << "\n";
    return 0;
}

bool sys_init()
{
    Sys::afont = new bfont(Sys::Renderer, std::string("The Strider.bmp"));
    
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
        Maps::load_background("background.png");
        
        Sys::myinput.Init();
        Sys::myself = nullptr;
        Sys::did_send_playerrequest = false;
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
        Sys::tems.push_back(&Net::think);
    #endif
    Sys::tems.push_back(&Sys::SDLEvents); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::ClientThink); // client/think
        Sys::tems.push_back(&Sys::Physics); // physics
    #endif
    Sys::tems.push_back(&Sys::RenderThings); // rendering
    Sys::tems.push_back(&do_hud); // rendering
    Sys::tems.push_back(&Sys::PresentScreen); // rendering
    
    return 1;
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

bool main_init()
{
	Sys::speeds.push_back(0);
	
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    
    Sys::MainWindow = SDL_CreateWindow("Benetnasch", 300, 300, Sys::shape.w, Sys::shape.h, SDL_WINDOW_SHOWN);
    if (Sys::MainWindow == nullptr)
        std::cout << "Could not create an SDL window: " << SDL_GetError() << std::endl;
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    LuaHUD = luaL_newstate();
    if (LuaHUD == nullptr)
        std::cout << "Could not load LUA. Sorry.\n";
    luaL_Reg funcs[] = 
    {
        {"print", &tolua_print},
        {NULL,NULL}
    };
    
    luaL_newlib(LuaHUD, funcs);
    lua_setglobal(LuaHUD, "bhud");
    
    int afwe = luaL_loadfile(LuaHUD, "scripts/hud.lua");
    if (afwe != 0)
        std::cout << "Could not load HUD script: " << afwe << std::endl;
    
    
    srand(time(NULL));
    
    SDL_PumpEvents();
    Sys::myinput.Init();
    
    if(faucnet_init() < 0)
        return 0;
    
    Sys::view_x = 0;
    Sys::view_y = 0;
    
    Net::init(0);
    
    Sys::server = new Net::Connection( "127.0.0.1", 9180 );
    Net::connections.push_back(Sys::server);
    Sys::server->send_or_resend_connection_request();
    
    Net::assign ( 1, SERVERMESSAGE::PLAYERINPUTS, &process_message_playerinputs );
    Net::assign ( 0, SERVERMESSAGE::ADDPLAYER, &process_message_addplayer );
    Net::assign ( 1, SERVERMESSAGE::PLAYERPOSITIONS, &process_message_playerpositions );
    Net::assign ( 0, SERVERMESSAGE::SERVEPLAYER, &process_message_serveplayer );
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
