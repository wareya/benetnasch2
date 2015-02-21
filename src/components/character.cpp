#include "gamecomponents.hpp"

namespace Sys
{
    Character::Character(entityid_t myEntity, double argx, double argy) : Component(myEntity), hspeed(0), vspeed(0), myself(false)
    {
        // movement hull
        hull = new Hull(myEntity, 12, 36, 10, 12);
        // hitboxes
        body = new Hull(myEntity, 12, 30, 10, 18);
        head = new Hull(myEntity, 8, 8, 12, 10);
        
        position = new Position(myEntity, argx, argy);
        
        sprite = new TexturedDrawable(Ent::New(), argx, argy, 0, 0);
        weaponsprite = new RotatingTexturedDrawable(sprite->entityID, argx, argy, -8, 8, 0, 20, 16);
        
        sprite->set_sprite(image_stand);
        weaponsprite->set_sprite(image_weapon);
        
        delete sprite->position;
        sprite->position = position;
        
        delete weaponsprite->position;
        weaponsprite->position = position;
        
        puts("making a character");
        
        Characters.add(this);
    }
    Character::~Character()
    {
        puts("2");
        delete hull;
        delete head;
        delete body;
        delete position;
        delete sprite;
        delete weaponsprite;
        Characters.remove(this);
    }
    void Character::center_on(float x, float y)
    {
        position->x = x - (hull->w+hull->xoffset)/2;
        position->y = y - (hull->h+hull->yoffset)/2;
    }
    float Character::center_x ()
    {
        return position->x + (hull->w+hull->xoffset)/2;
    }
    float Character::center_y ()
    {
        return position->y + (hull->h+hull->yoffset)/2;
    }
}
