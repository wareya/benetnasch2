#include "drawingcomponents.hpp"

namespace Sys
{
    AnimatedTexturedDrawable::AnimatedTexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, int arglength, double argwidth, double argheight)
    : TexturedDrawable(myEntity, argx, argy, argxoffset, argyoffset, 1), length(arglength), index(0), width(argwidth), height(argheight)
    {
        AnimatedTexturedDrawables.add(this);
    }
    AnimatedTexturedDrawable::~AnimatedTexturedDrawable()
    {
        if(position->entityID == entityID) delete position;
        
        AnimatedTexturedDrawables.remove(this);
    }
}
