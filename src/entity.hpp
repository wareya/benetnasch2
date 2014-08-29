#ifndef INCLUDE_BENTITY
#define INCLUDE_BENTITY

#include <nall/set.hpp>

typedef long long entityid_t;

namespace Ent
{
    extern nall::set<entityid_t> Used; // IDs which are currently being used
    extern nall::set<entityid_t> Freed; // IDs which were used at some point, but no longer are
    extern entityid_t LowestUnused; // The lowest ID which has never been used yet
    
    entityid_t New();
    // free a given ID -- returns false if id doesn't exist
    bool Free(entityid_t id);
    bool Exists(entityid_t id);
}

#endif
