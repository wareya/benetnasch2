#include "components.hpp"
#include <nall/set.hpp>

namespace Sys
{
    // Read components.hpp
    Component::Component(entityid_t myEntity) : entityID(myEntity)
    { }
    Component::~Component()
    { }
}
