#include "renderers.hpp"
#include "commonrenderdep.hpp"
#include "../components/textwindow.hpp"

namespace Sys
{
    namespace Renderers
    {
        //void renderTextureInternalEX( SDL_Texture *tex, SDL_Renderer *renderer, int x, int y, int w, int h, int sx, int sy, int sw, int sh )
        void renderWindowImage (SDL_Texture * image, int x, int y, int width, int height)
        {
            int spriteheight, spritewidth;
            SDL_QueryTexture( image, NULL, NULL, &spritewidth, &spriteheight );
            renderTextureInternalEX( image, Sys::Renderer, x, y         , width, 2       , 0, 0             , spritewidth, 2             );
            renderTextureInternalEX( image, Sys::Renderer, x, y+2       , width, height-4, 0, 2             , spritewidth, spriteheight-4);
            renderTextureInternalEX( image, Sys::Renderer, x, y+height-2, width, 2       , 0, spriteheight-2, spritewidth, 2             );
        }
        bool DrawTextWindows(float _unused_x, float _unused_y) // topleft corner position in world
        {
            auto textheight = Sys::afont->h;
            for(auto window : Sys::TextWindows)
            {
                if(!window->visible)
                    continue;
                
                //renderTexture( window->background, Sys::Renderer, window->x_onscreen, window->y_onscreen, 1, 0 );
                renderWindowImage( window->background, window->x_onscreen, window->y_onscreen, window->width_pixels, window->height_pixels );
                
                unsigned height_lines = (window->height_pixels-2)/textheight;
                unsigned number_lines = window->lines.size();
                auto lines = height_lines > number_lines ? number_lines : height_lines;
                
                for(unsigned i = 1; i <= lines; i++)
                {
                    auto position = window->y_onscreen+window->height_pixels;
                    position -= i*textheight;
                    auto which = lines - i;
                    renderText(window->x_onscreen+2, position, window->lines[which].data(), Sys::afont);
                }
            };
            
            for(auto window : Sys::TextBoxes)
            {
                if(!window->visible)
                    continue;
                
                //renderTexture( window->background, Sys::Renderer, window->x_onscreen, window->y_onscreen, 1, 0 );
                renderWindowImage( window->background, window->x_onscreen, window->y_onscreen, window->width_pixels, window->height_pixels );
                
                renderText(window->x_onscreen+2, window->y_onscreen+2, window->line.data(), Sys::afont);
            };
            return false;
        }
    }
}
