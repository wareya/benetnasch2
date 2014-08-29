#ifndef INCLUDED_BPHYSICS
#define INCLUDED_BPHYSICS

#include <vector>
#include "components/allcomponents.hpp"
#include "blib.hpp" // for btime in order for delta

namespace Sys
{
    namespace Physicsers
    {
        extern double delta;
        extern bool delta_is_too_damn_low;
        
        /* characters.cpp */
        bool MoveCharacters();
        
        /* bullets.cpp */
        bool MoveBullets();
    }
    bool UpdateDelta();
    
    bool Physics();
}
#endif
