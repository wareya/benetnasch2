#ifndef INCLUDE_BTEXTWINDOW
#define INCLUDE_BTEXTWINDOW

#include "gamecomponents.hpp"
#include "../bengine.hpp"
#include <deque>

namespace Sys
{
    struct TextWindow : public Component
    {
        TextWindow(entityid_t myEntity);
        ~TextWindow();
        
        unsigned scrollback_size;
        unsigned width_pixels;
        unsigned height_pixels;
        unsigned x_onscreen;
        unsigned y_onscreen;
        
        SDL_Texture * background;
        
        std::deque<std::string> lines;
        
        void force_purge_extra_scrollback();
        void set_scrollback(unsigned);
        void append_line(std::string);
    };
    extern Collection<TextWindow> TextWindows;
}
#endif
