#include "primitives.hpp"

namespace Sys
{
    // Constructor will add this new instance to its list
    Position::Position(entityid_t myEntity, double argx, double argy) : Component(myEntity), x(argx), y(argy)
    {
        Positions.add(this);
    }
    Position::~Position()
    {
        Positions.remove(this);
    }
    
    Hull::Hull(entityid_t myEntity, double argw, double argh, double argxoffset, double argyoffset) : Component(myEntity), w(argw), h(argh), xoffset(argxoffset), yoffset(argyoffset)
    {
        Hulls.add(this);
    }
    Hull::~Hull()
    {
        Hulls.remove(this);
    }
}
