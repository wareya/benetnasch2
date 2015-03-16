#include "serverplayer.hpp"
#include "genericallocator.hpp"

// read serverplayer.hpp
namespace Sys
{
    namespace PlayerList
    {
        std::deque<ServerPlayer*> Slots;
        
        unsigned AddPlayer(Net::Connection * connection, Player * player)
        {
            auto r = new ServerPlayer({connection, player});
            Slots.push_back(r);
            return Slots.size()-1;
        }
        void Remove(playerid pid)
        {
            if ( pid > Slots.size() )
                return; // TODO return error
            Slots.erase(Slots.begin()+pid);
        }
        void Clear()
        {
            while ( Slots.size() > 0 )
            {
                delete Slots[0];
                Slots.pop_front();
            }
        }
        ServerPlayer * FromConnection(Net::Connection * connection)
        {
            unsigned i = 0;
            for( ; i < Slots.size(); ++i )
            {
                if ( Slots[i]->connection == connection )
                    return Slots[i];
            }
            return NULL;
        }
        playerid IdFromConnection(Net::Connection * connection)
        {
            unsigned i = 0;
            for( ; i < Slots.size(); ++i )
            {
                if ( Slots[i]->connection == connection )
                    return i;
            }
            return -1;
        }
    }
}
