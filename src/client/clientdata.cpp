#include "clientdata.hpp"

#include "../input.hpp"
#include "../components/player.hpp"
#include "../network.hpp"

namespace Sys
{
    // input before being sent to server
    Input::ClientInput myinput;
    // pointer to own player component
    Player * myself;
    // vector of known own speeds for debug display
    std::vector<float> speeds;
    // server connection
    Net::Connection * server;
    // whether we asked the server to give us a player slot
    bool did_send_playerrequest;
}

