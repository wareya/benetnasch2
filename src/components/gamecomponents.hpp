#ifndef INCLUDE_BGAMECOMPONENTS
#define INCLUDE_BGAMECOMPONENTS

#include "../components.hpp"
#include "drawingcomponents.hpp"

namespace Sys
{
    // Character component
    struct Character : public Component
    {
        Character(entityid_t myEntity, double argx, double argy);
        ~Character();
        Position * position;
        Hull * hull;
        Hull * head;
        Hull * body;
        // TODO: Separate somehow to remove "gameponents" dependency on "drawingcomponents"
        TexturedDrawable * sprite;
        RotatingTexturedDrawable * weaponsprite;
        const char * image_stand = "sprites/stand.png";
        const char * image_jump = "sprites/jump.png";
        const char * image_run = "sprites/run.png";
        const char * image_leanF = "sprites/leanF.png";
        const char * image_leanB = "sprites/leanB.png";
        const char * image_weapon = "sprites/gun.png";
        
        double hspeed, vspeed;
        bool myself;
        
        void center_on(float x, float y);
        float center_x ();
        float center_y ();
    };
    extern Collection<Character> Characters;
    
    struct Bullet : public Component
    {
        Bullet(entityid_t myEntity, double argx, double argy, double argh, double argv, double argd);
        ~Bullet();
        Position * position;
        Position * lastposition;
        float hspeed;
        float vspeed;
        float damage;
        float life;
    };
    extern Collection<Bullet> Bullets;
    
    // misnomer: actually a wallmask chunk
    struct BoxDrawable : public Component
    {
        BoxDrawable(entityid_t myEntity, double argx, double argy, double argw, double argh);
        ~BoxDrawable();
        Position * position;
        Hull * hull;
        double xoffset, yoffset;
        
        SDL_Rect* getShape(float x, float y);
        
        private:
            SDL_Rect shape;
    };
    extern Collection<BoxDrawable> BoxDrawables;
}

#endif
