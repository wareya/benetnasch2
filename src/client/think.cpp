#include "think.hpp"

#include "clientdata.hpp"
#include "../physics.hpp" // delta_is_too_damn_low
#include "../network.hpp"
#include "../netconst.hpp"
#include "../blib.hpp"

namespace Sys
{
    double lastInputUpdate;
    bool ClientThink()
    {
        if(!server)
            return 0;
        double now = Time::get_us();
        if(server->ready)
        {
            if(!did_send_playerrequest)
            {
                puts("telling server I want it to spawn me");
                auto request = buffer_create();
                write_ubyte(request, 6); // len
                write_string(request, "Wareya");
                Net::send(Sys::server, 0, CLIENTMESSAGE::PLAYERREQUEST, request);
                buffer_destroy(request);
                did_send_playerrequest = true;
            }
            if(!Physicsers::delta_is_too_damn_low) // TODO: Replace with rate
            {
                if(Sys::myself and (now - lastInputUpdate)/1000 >= 20)// NOTE: 50 per second
                {
                    lastInputUpdate = now; // TODO: Make more accurate (would currently result in a lower-than-intended update rate)
                    
                    Sys::myinput.Update();
                    
                    auto input = buffer_create();
                    write_ushort(input, Sys::myinput.myplayerinput.getInputsAsBitfield());
                    write_ushort(input, Sys::myinput.myplayerinput.aimDirection/360.0*0x10000);
                    write_ubyte(input, Sys::myinput.myplayerinput.aimDistance);
                    Net::send(Sys::server, 1, CLIENTMESSAGE::INPUT, input);
                    buffer_destroy(input);
                    
                    if(Sys::myself->character)
                    {
                        Sys::speeds.push_back(Sys::myself->character->hspeed);
                        while ( Sys::speeds.size() > 300 )
                            Sys::speeds.erase ( speeds.begin() );
                    }
                }
            }
        }
        return 0;
    }
}
