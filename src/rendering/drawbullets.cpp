#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawBullets(float x, float y)
        {
            for(auto bullet : Sys::Bullets)
            {
                SDL_RenderDrawLine(Sys::Renderer, bullet->position->x-x, bullet->position->y-y, bullet->lastposition->x-x, bullet->lastposition->y-y);
            };
            return false;
        }
    }
}
