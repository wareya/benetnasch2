#ifndef INCLUDE_NETCONST
#define INCLUDE_NETCONST

namespace CLIENTMESSAGE
{
    enum
    {
        INPUT,
        PLAYERREQUEST
    };
}
namespace SERVERMESSAGE
{
    enum
    {
        PLAYERINPUT,
        SPAWNNEWPLAYER,
        PLAYERPOSITIONS,
        PLAYERLIST
    };
}
namespace MESSAGE
{
    enum
    {
        DROPPABLE
    };
}

#endif
