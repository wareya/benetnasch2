#include "renderers.hpp"
#include "commonrenderdep.hpp"
#include "../components/textwindow.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawTextWindows(float _unused_x, float _unused_y) // topleft corner position in world
        {
            auto textheight = Sys::afont->h;
            for(auto window : Sys::TextWindows)
            {
                renderTexture( window->background, Sys::Renderer, window->x_onscreen, window->y_onscreen, 1, 0 );
                
                unsigned height_lines = window->height_pixels/textheight;
                unsigned number_lines = window->lines.size();
                auto lines = height_lines > number_lines ? number_lines : height_lines;
                
                for(unsigned i = 1; i <= lines; i++)
                {
                    auto position = window->y_onscreen+window->height_pixels;
                    position -= i*textheight;
                    auto which = lines - i;
                    renderText(window->x_onscreen, position, window->lines[which].data(), Sys::afont);
                }
            };
            return false;
        }
    }
}
