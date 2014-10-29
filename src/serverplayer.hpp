#ifndef INCLUDE_SERVERPLAYER
#define INCLUDE_SERVERPLAYER

#include "components/player.hpp"
#include "network.hpp"

namespace Sys
{
    typedef int playerid;
    struct ServerPlayer
    {
        Net::Connection * connection;
        Player * player;
        playerid id;
    };
    namespace ServerPlayers
    {
        playerid Add(Net::Connection * connection, Player * player);
        void Remove(playerid);
        ServerPlayer * FromConnection(Net::Connection * connection);
    }
}

#endif
