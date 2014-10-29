#ifndef INCLUDE_CLIENTDATA
#define INCLUDE_CLIENTDATA

#include <vector>

#include "../input.hpp"
#include "../components/player.hpp"
#include "../network.hpp"

namespace Sys
{
    extern Input::ClientInput myinput;
    extern Player * myself;
    extern std::vector<float> speeds;
    extern Net::Connection * server;
    extern bool did_send_playerrequest;
}

#endif
