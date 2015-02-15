#ifndef INCLUDE_BTEXTWINDOW
#define INCLUDE_BTEXTWINDOW

#include "gamecomponents.hpp"
#include "../bengine.hpp"
#include <deque>

namespace Sys
{
    struct TextBox : public Component
    {
        TextBox(entityid_t myEntity);
        ~TextBox();
        
        unsigned width_pixels;
        unsigned height_pixels;
        unsigned x_onscreen;
        unsigned y_onscreen;
        bool visible;
        
        SDL_Texture * background;
        
        std::string line;
    };
    extern Collection<TextBox> TextBoxes;
    
    struct TextWindow : public Component
    {
        TextWindow(entityid_t myEntity);
        ~TextWindow();
        
        unsigned scrollback_size;
        unsigned width_pixels;
        unsigned height_pixels;
        unsigned x_onscreen;
        unsigned y_onscreen;
        bool visible;
        
        SDL_Texture * background;
        
        std::deque<std::string> lines;
        
        void force_purge_extra_scrollback();
        void set_scrollback(unsigned);
        void append_line(std::string);
    };
    extern Collection<TextWindow> TextWindows;
}
#endif
