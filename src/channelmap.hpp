#ifndef INCLUDE_CHANNELMAP
#define INCLUDE_CHANNELMAP

#include "netconst.hpp"
#include <map>
#include <vector>

namespace Net
{
    // map channel : incrementors
    typedef unsigned short msg;
    typedef unsigned int inc;
    extern std::map<msg, std::vector<inc>> channel_incrementors;
    extern std::map<msg, inc> message_dependencies;
    
    void setup_channels();
}

#endif
