#include "drawingcomponents.hpp"

namespace Sys
{
    RotatingTexturedDrawable::RotatingTexturedDrawable( entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, double argangle, double argxorigin, double argyorigin)
    : TexturedDrawable(myEntity, argx, argy, argxoffset, argyoffset, 1), angle(argangle), xorigin(argxorigin), yorigin(argyorigin)
    {
        RotatingTexturedDrawables.add(this);
    }
    RotatingTexturedDrawable::~RotatingTexturedDrawable()
    {
        if(position->entityID == entityID) delete position;
        
        RotatingTexturedDrawables.remove(this);
    }
}
