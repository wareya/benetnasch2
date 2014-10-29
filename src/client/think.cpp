#include "think.hpp"

#include "clientdata.hpp"
#include "../physics.hpp" // delta_is_too_damn_low
#include "../network.hpp"
#include "../netconst.hpp"
#include "../blib.hpp"

namespace Sys
{
    bool ClientThink()
    {
        if(!Physicsers::delta_is_too_damn_low)
        {
            Sys::myinput.Update();
            
            auto input = buffer_create();
            write_ushort(input, Sys::myinput.myplayerinput.getInputsAsBitfield());
            write_ushort(input, Sys::myinput.myplayerinput.aimDirection/360.0*0x10000);
            write_ubyte(input, Sys::myinput.myplayerinput.aimDistance);
            Net::send(Sys::server, 0, CLIENTMESSAGE::INPUT, input);
            buffer_destroy(input);
            
            Sys::physics_frames_since_input_cycle += 1;
            
            Sys::speeds.push_back(Sys::myself->character->hspeed);
            while ( Sys::speeds.size() > 300 )
                Sys::speeds.erase ( speeds.begin() );
        }
        return 0;
    }
}
