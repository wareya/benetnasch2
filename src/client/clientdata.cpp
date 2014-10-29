#include "clientdata.hpp"

#include "../input.hpp"
#include "../components/player.hpp"
#include "../network.hpp"

namespace Sys
{
    Input::ClientInput myinput;
    Player * myself;
    std::vector<float> speeds;
    Net::Connection * server;
    bool did_send_playerrequest;
}

