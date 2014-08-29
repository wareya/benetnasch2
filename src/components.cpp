#include "components.hpp"
#include <nall/set.hpp>

namespace Sys
{ 
    Component::Component(entityid_t myEntity) : entityID(myEntity)
    { }
    Component::~Component()
    { }
}
