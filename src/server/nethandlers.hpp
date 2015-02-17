#ifndef INCLUDE_SERVERNET
#define INCLUDE_SERVERNET

#include "../network.hpp"
#include "../serverplayer.hpp"

namespace Sys
{
    void process_message_input(Net::Connection * connection, double buffer);

    double build_message_serveplayer(Sys::ServerPlayer * targetplayer, double buffer);

    void process_message_playerrequest(Net::Connection * connection, double buffer);
    
    void add_processors();
    void clear_processors();
}
#endif
