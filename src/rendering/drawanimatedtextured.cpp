#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawAnimatedTextured(float x, float y) // topleft corner position
        {
            for(auto drawable : Sys::AnimatedTexturedDrawables)
            {
                if(!drawable->visible)
                    continue;
                double index = int(drawable->index);
                while(index >= drawable->length)
                    index -= drawable->length;
                while(index < 0)
                    index += drawable->length;
                index *= drawable->width;
                renderTexture( drawable->sprite, Sys::Renderer, drawable->position->x-x, drawable->position->y-y, drawable->width, drawable->height, index, drawable->flip );
            };
            return false;
        }
    }
}
