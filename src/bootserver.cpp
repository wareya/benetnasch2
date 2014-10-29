#include "bengine.hpp"
#include "blib.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "physics.hpp"
#include "input.hpp"
#include "network.hpp"
#include "netconst.hpp"

bool sys_init()
{
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    Sys::tems.push_back(&Net::think);
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
        Sys::tems.push_back(&Sys::Physics); // physics
    #endif
    
    return 1;
}

void process_message_input(Net::Connection * connection, double buffer)
{
    std::cout << "SERVER: Got an input, size: " << buffer_size(buffer) << " IP: " << connection->hostname << " Port: " << connection->port << "\n";
    
    auto response = buffer_create();
    write_ushort(response, read_ushort(buffer)); // key
    write_ushort(response, read_ushort(buffer)); // angle
    write_ubyte(response, read_ubyte(buffer)); // distance
    Net::send(connection, 1, SERVERMESSAGE::PLAYERINPUT, response);
    buffer_destroy(response);
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
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
