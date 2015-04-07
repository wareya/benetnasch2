#include "channelmap.hpp"
#include "network.hpp"

using namespace CLIENTMESSAGE;
using namespace SERVERMESSAGE;
using namespace CHANNEL;

namespace Net
{
    // map channel : incrementors
    std::map<msg, std::vector<inc>> channel_incrementors;
    std::map<msg, inc> message_dependencies;
    std::map<msg, bool> message_droppability;
    
    void setup_channels()
    {
        channel_incrementors = 
        {
            {ADDPLAYER, {PLAYERLIST}},
            {REMOVEPLAYER, {PLAYERLIST}}
        };
        message_dependencies = 
        {
            {PLAYERPOSITIONS, PLAYERLIST},
            {PLAYERINPUTS, PLAYERLIST}
        };
        message_droppability = 
        {
            {INPUT, DROPPABLE},
            {PLAYERREQUEST, UNDROPPABLE},
            {PLAYERINPUTS, DROPPABLE},
            {PLAYERPOSITIONS, DROPPABLE},
            {SERVEPLAYER, UNDROPPABLE},
            {ADDPLAYER, UNDROPPABLE},
            {REMOVEPLAYER, UNDROPPABLE}
        };
    }
}
