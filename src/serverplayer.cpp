#include "serverplayer.hpp"
#include "genericallocator.hpp"
#include <vector>

namespace Sys
{
    namespace PlayerList
    {
        std::vector<ServerPlayer*> Slots;
        
        unsigned AddPlayer(Net::Connection * connection, Player * player)
        {
            auto r = new ServerPlayer({connection, player});
            Slots.push_back(r);
            return Slots.size()-1;
        }
        void Remove(unsigned int pid)
        {
            if ( pid > Slots.size() )
                return;
            Slots.erase(Slots.begin()+pid);
        }
        ServerPlayer * FromConnection(Net::Connection * connection)
        {
            unsigned i = 0;
            for(; i < Slots.size(); ++i)
            {
                if ( Slots[i]->connection == connection )
                    return Slots[i];
            }
            return NULL;
        }
    }
}
