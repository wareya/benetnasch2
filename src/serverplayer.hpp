#ifndef INCLUDE_SERVERPLAYER
#define INCLUDE_SERVERPLAYER

#include "components/player.hpp"
#include "network.hpp"

namespace Sys
{
    typedef unsigned char playerid;
    struct ServerPlayer
    {
        Net::Connection * connection;
        Player * player;
        playerid id;
    };
    namespace ServerPlayers
    {
        extern std::vector<ServerPlayer*> ServerPlayers;
        playerid Add(Net::Connection * connection, Player * player);
        void Remove(playerid);
        ServerPlayer * FromConnection(Net::Connection * connection);
        ServerPlayer * FromPid(playerid pid);
    }
}

#endif
