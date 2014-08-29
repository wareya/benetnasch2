#include "renderers.hpp"
#include "commonrenderdep.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawCharacterDebug(float x, float y)
        {
            for(auto c : Sys::Characters)
            {
                SDL_Rect shape;
                
                auto render = [&shape, x, y, c](Hull * arg) {
                    shape = {int(floor(arg->xoffset+c->position->x-x)), int(floor(arg->yoffset+c->position->y-y)),
                             int(round(arg->w)), int(round(arg->h))};
                    SDL_RenderDrawRect( Sys::Renderer, &shape );
                };
                SDL_SetRenderDrawColor(Sys::Renderer, 255, 0, 127, 255);
                render(c->hull);
                SDL_SetRenderDrawColor(Sys::Renderer, 0, 127, 255, 255);
                render(c->head);
                render(c->body);
            };
            return false;
        }
    }
}
