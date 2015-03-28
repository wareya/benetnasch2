#ifndef INCLUDE_NETCONST
#define INCLUDE_NETCONST

namespace CLIENTMESSAGE
{
    enum
    {
        INPUT, // 
        PLAYERREQUEST // gimme a player
    };
}
namespace SERVERMESSAGE
{
    enum
    {
        invalid_message1,
        invalid_message2,
        PLAYERINPUTS, // list of every living player's inputs
        PLAYERPOSITIONS, // list of every living player's positions and inputs
        SERVEPLAYER, // list of every player and their name, for joining clients
        ADDPLAYER, // add player with name to end of player list
        REMOVEPLAYER // remove given player from player list
    };
}
namespace MESSAGE
{
    enum
    {
        DROPPABLE
    };
}
namespace CHANNEL
{
    enum
    {
        PLAYERLIST,
        NUMCHANNELS
    };
}
#endif
