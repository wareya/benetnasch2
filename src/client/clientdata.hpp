#ifndef INCLUDE_CLIENTDATA
#define INCLUDE_CLIENTDATA

#include <vector>

#include "../input.hpp"
#include "../components/player.hpp"

namespace Sys
{
    extern Input::ClientInput myinput;
    extern Player * myself;
    extern std::vector<float> speeds;
}

#endif
