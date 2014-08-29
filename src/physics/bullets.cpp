#include "subroutines.hpp"

namespace Sys
{
    namespace Physicsers
    {
        bool MoveBullets()
        {
            float gravity = 800*delta;
            std::vector<Bullet *> marked_for_removal;
            for(auto bullet : Sys::Bullets)
            {
                auto &x = bullet->position->x;
                auto &y = bullet->position->y;
                
                *bullet->lastposition = *bullet->position;
                
                auto &h = bullet->hspeed;
                auto &v = bullet->vspeed;
                
                v += gravity;
                
                x += h * delta;
                y += v * delta;
                
                bullet->life -= delta;
                
                if(bullet->life < 0 or line_meeting_which(bullet->lastposition->x, bullet->lastposition->y, x, y).size()>0)
                {
                    marked_for_removal.push_back(bullet);
                }
            }
            for(auto bullet : marked_for_removal)
                delete bullet;
            return false;
        }
    }
}
