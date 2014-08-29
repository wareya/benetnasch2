#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawBoxes(float x, float y)
        {
            SDL_SetRenderDrawColor( Sys::Renderer, 255, 255, 255, 255 );
            for(auto drawable : Sys::BoxDrawables)
            {
                SDL_RenderFillRect( Sys::Renderer, drawable->getShape(x, y) );
            };
            return false;
        }
    }
}
