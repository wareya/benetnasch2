#include "think.hpp"

#include "clientdata.hpp"
#include "../physics.hpp" // delta_is_too_damn_low

namespace Sys
{
    bool ClientThink()
    {
        if(!Physicsers::delta_is_too_damn_low)
        {
            Sys::myinput.Update();
            Sys::myself->input = Sys::myinput.myplayerinput; // TODO: Send over network
        }
        return 0;
    }
}
