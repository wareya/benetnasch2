#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawTextured(float x, float y) // topleft corner position
        {
            for(auto drawable : Sys::TexturedDrawables)
            {
                renderTexture( drawable->sprite, Sys::Renderer, drawable->position->x-x, drawable->position->y-y, 1, drawable->flip );
            };
            return false;
        }
    }
}
