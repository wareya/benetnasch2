#include "channelmap.hpp"

using namespace CLIENTMESSAGE;
using namespace SERVERMESSAGE;
using namespace CHANNEL;

namespace Net
{
    // map channel : incrementors
    std::map<msg, std::vector<inc>> channel_incrementors;
    std::map<msg, inc> message_dependencies;
    
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
    }
}
