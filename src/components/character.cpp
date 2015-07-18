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
        
        #ifdef CLIENT
            stand = new TexturedDrawable(Ent::New(), argx, argy, 0, 0);
            run = new AnimatedTexturedDrawable(stand->entityID, argx, argy, 0, 0, 4, 32, 48);
            weaponsprite = new RotatingTexturedDrawable(stand->entityID, argx, argy, -8, 8, 0, 20, 16);
            
            stand->set_sprite(image_stand);
            run->set_sprite(image_run);
            run->visible = false;
            weaponsprite->set_sprite(image_weapon);
            
            delete stand->position;
            stand->position = position;
            
            delete run->position;
            run->position = position;
            
            delete weaponsprite->position;
            weaponsprite->position = position;
        #endif
        
        puts("making a character");
        
        Characters.add(this);
    }
    Character::~Character()
    {
        delete hull;
        delete head;
        delete body;
        delete position;
        #ifdef CLIENT
            delete stand;
            delete run;
            delete weaponsprite;
        #endif
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
