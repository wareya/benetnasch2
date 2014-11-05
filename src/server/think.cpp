#include "think.hpp"

#include "../network.hpp"
#include "../netconst.hpp"
#include "../blib.hpp"
#include "../components/player.hpp"
#include "../serverplayer.hpp"

#include <iostream>

namespace Sys
{
    double lastQuickUpdate;
    bool ServerThink()
    {
        double now = Time::get_us();
        if((now - lastQuickUpdate)/1000 >= 200) // NOTE: 5 per second
        {
            lastQuickUpdate = now; // TODO: Make more accurate (would currently result in a lower-than-intended update rate)
            double theresponse = buffer_create();
            for(auto serverplayer : ServerPlayers::ServerPlayers)
            {
                if(serverplayer->player->character)
                {
                    write_ubyte(theresponse, serverplayer->id);
                    write_ushort(theresponse, serverplayer->player->character->position->x*10);
                    write_ushort(theresponse, serverplayer->player->character->position->y*10);
                }
            }
            for(auto serverplayer : ServerPlayers::ServerPlayers)
            {
                send(serverplayer->connection, 1, SERVERMESSAGE::PLAYERPOSITIONS, theresponse);
            }
            buffer_destroy(theresponse);
        }
        return 0;
    }
}
