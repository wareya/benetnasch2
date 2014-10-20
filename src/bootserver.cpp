#include "blib.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "physics.hpp"
#include "speed.hpp"
#include "input.hpp"
#include "network.hpp"


bool sys_init()
{
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
        
        auto me = new Sys::Character(Ent::New(), Maps::width/2, Maps::height/2);
        me->myself = true;
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
        Sys::tems.push_back(&Sys::Physics); // physics
    #endif
    
    return 1;
}

bool main_init()
{
	speeds.push_back(0);
	
    Net::init(4192);
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
