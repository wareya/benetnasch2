//#define USE_SOURCE_HEADER_ENTITY
#include "entity.hpp"
#include "genericallocator.hpp"
#include <iostream> // debugging

namespace Ent
{
    GenericAllocator<entityid_t> allocator;
    
    entityid_t New()
    {
        return allocator.New();
    }
    // free a given ID -- returns false if id doesn't exist
    bool Free(entityid_t id) //perf: 3xlogn
    {
        return allocator.Free(id);
    }
    bool Exists(entityid_t id)
    {
        return allocator.Exists(id);
    }
}
