#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawBackground(float x, float y)
        {
            for(auto drawable : Sys::BackgroundDrawables)
            {
                renderTexture( drawable->sprite, Sys::Renderer, drawable->position->x-x, drawable->position->y-y, 4, false );
            };
            return false;
        }
    }
}
