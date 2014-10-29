#ifndef INCLUDE_BENTITY
#define INCLUDE_BENTITY

typedef long long entityid_t;

namespace Ent
{
    entityid_t New();
    // free a given ID -- returns false if id doesn't exist
    bool Free(entityid_t id);
    bool Exists(entityid_t id);
}

#endif
