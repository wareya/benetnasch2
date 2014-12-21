#include "bengine.hpp"
#include "blib.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "physics.hpp"
#include "input.hpp"
#include "network.hpp"
#include "netconst.hpp"
#include "serverplayer.hpp"
#include "components/gamecomponents.hpp"
#include "components/player.hpp"
#include "server/think.hpp"
#include "server/nethandlers.hpp"

// push systems into the mainloop
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
    
    return 1; // remove sys_init from mainloop
}

bool main_init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    SDL_PumpEvents();
    
    // load faucnet and assign function pointers
    if(faucnet_init() < 0)
        return 0;
    
    // register net message handlers from server/nethandlers.cpp
    Sys::add_processors();
    
    // start listening for clients on 9180
    Net::init(9180);
    
    // get mainloop ready to get itself ready
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
