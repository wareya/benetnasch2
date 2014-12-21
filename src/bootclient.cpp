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
#include "client/nethandlers.hpp"
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
    
    Sys::add_processors();
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
