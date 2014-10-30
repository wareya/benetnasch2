#include "serverplayer.hpp"
#include "genericallocator.hpp"
#include <vector>

namespace Sys
{
    namespace ServerPlayers
    {
        std::vector<ServerPlayer*> ServerPlayers;
        GenericAllocator<playerid> pids;
        
        playerid Add(Net::Connection * connection, Player * player)
        {
            auto r = new ServerPlayer({connection, player, pids.New()});
            ServerPlayers.push_back(r);
            return r->id;
        }
        void Remove(int pid)
        {
            unsigned i = 0;
            for(; i < ServerPlayers.size(); ++i)
            {
                if ( ServerPlayers[i]->id == pid )
                    break;
            }
            if(i < ServerPlayers.size())
            {
                pids.Free(pid);
                ServerPlayers.erase(ServerPlayers.begin()+i);
            }
        }
        ServerPlayer * FromConnection(Net::Connection * connection)
        {
            unsigned i = 0;
            for(; i < ServerPlayers.size(); ++i)
            {
                if ( ServerPlayers[i]->connection == connection )
                    return ServerPlayers[i];
            }
            return NULL;
        }
        ServerPlayer * FromPid(playerid pid)
        {
            unsigned i = 0;
            for(; i < ServerPlayers.size(); ++i)
            {
                if ( ServerPlayers[i]->id == pid )
                    return ServerPlayers[i];
            }
            return NULL;
        }
    }
}
