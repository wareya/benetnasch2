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
        if((now - lastQuickUpdate)/1000 >= 25) // NOTE: 40 per second
        {
            auto update = SERVERMESSAGE::PLAYERINPUTS;
            if(fabs((now - lastQuickUpdate)/1000) >= 200) // do positions too (5 per second)
            {
                update = SERVERMESSAGE::PLAYERPOSITIONS;
                lastQuickUpdate = now; // TODO: Make more accurate (would currently result in a lower-than-intended update rate)
            }
            double theresponse = buffer_create();
            for ( unsigned i = 0; i < Sys::PlayerList::Slots.size(); ++i )
            {
                auto player = Sys::PlayerList::Slots[i]->player;
                auto character = player->character;
                if(character)
                {
                    write_ubyte(theresponse, i);
                    if(update == SERVERMESSAGE::PLAYERPOSITIONS)
                    {
                        write_ushort(theresponse, character->position->x*10);
                        write_ushort(theresponse, character->position->y*10);
                        write_short(theresponse, character->hspeed*4);
                        write_short(theresponse, character->vspeed*4);
                    }
                    write_ushort(theresponse, player->input.netkeys); // key
                    write_ushort(theresponse, player->input.netaimdir); // angle
                    write_ubyte(theresponse, player->input.netaimdist); // distance
                }
            }
            for(auto serverplayer : PlayerList::Slots)
                Net::send(serverplayer->connection, update, theresponse);
            
            buffer_destroy(theresponse);
        }
        return 0;
    }
}
