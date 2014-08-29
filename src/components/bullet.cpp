#include "gamecomponents.hpp"

namespace Sys
{
    Bullet::Bullet(entityid_t myEntity, double argx, double argy, double argh, double argv, double argd) : Component(myEntity), hspeed(argh), vspeed(argv), damage(argd), life(1)
    {
        position = new Position(myEntity, argx, argy);
        lastposition = new Position(myEntity, argx, argy);
        Bullets.add(this);
    }
    Bullet::~Bullet()
    {
        delete position;
        delete lastposition;
        Bullets.remove(this);
    }
}
