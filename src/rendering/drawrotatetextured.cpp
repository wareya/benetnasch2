#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawRotateTextured(float x, float y) // topleft corner position
        {
            for(auto drawable : Sys::RotatingTexturedDrawables)
            {
                renderTexture( drawable->sprite, Sys::Renderer, drawable->position->x+drawable->xoffset-x, drawable->position->y+drawable->yoffset-y, 1, -drawable->angle, drawable->xorigin, drawable->yorigin, drawable->flip );
            };
            return false;
        }
    }
}
