#ifndef INCLUDE_CLIENTNET
#define INCLUDE_CLIENTNET

#include "../network.hpp"

namespace Sys
{
    void process_message_addplayer(Net::Connection * connection, double buffer);

    void process_message_playerpositions(Net::Connection * connection, double buffer);

    void process_message_playerinputs(Net::Connection * connection, double buffer);

    void process_message_serveplayer(Net::Connection * connection, double buffer);

    void add_processors();
}
#endif
