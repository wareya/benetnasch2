#ifndef INCLUDE_SERVERPLAYER
#define INCLUDE_SERVERPLAYER

#include "components/player.hpp"
#include "network.hpp"

#include <deque>

namespace Sys
{
    typedef unsigned char playerid;
    struct ServerPlayer
    {
        Net::Connection * connection;
        Player * player;
        playerid id;
    };
    namespace PlayerList
    {
        extern std::deque<ServerPlayer*> Slots;
        unsigned AddPlayer(Net::Connection * connection, Player * player);
        void Remove(playerid);
        void Clear();
        ServerPlayer * FromConnection(Net::Connection * connection);
        playerid IdFromConnection(Net::Connection * connection);
    }
}

#endif
