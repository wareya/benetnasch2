#include "gamecomponents.hpp"

namespace Sys
{
    // misnomer: actually a wallmask chunk
    BoxDrawable::BoxDrawable(entityid_t myEntity, double argx, double argy, double argw, double argh) : Component(myEntity)
    {
        position = new Position(myEntity, argx, argy);
        hull = new Hull(myEntity, argw, argh, 0, 0);
        BoxDrawables.add(this);
    }
    BoxDrawable::~BoxDrawable()
    {
        delete position;
        delete hull;
        BoxDrawables.remove(this);
    }
    SDL_Rect * BoxDrawable::getShape(float x, float y) // offsets
    {
        shape = {int(ceil(position->x+hull->xoffset-x)),
                 int(ceil(position->y+hull->yoffset-y)),
                 int(round(hull->w)),
                 int(round(hull->h))};
        return &shape;
    }
}
