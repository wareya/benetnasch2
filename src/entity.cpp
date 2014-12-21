//#define USE_SOURCE_HEADER_ENTITY
#include "entity.hpp"
#include "genericallocator.hpp"
#include <iostream> // debugging

// Entities aren't reaallly a thing. Their primary use is to distinguish
// when complex component structures share components. Check out character.cpp
// for an example, with it overriding the "position" of its drawables.
// Read textureddrawable's destructor for where entityid is actually used.
namespace Ent
{
    // Read entity.hpp
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
