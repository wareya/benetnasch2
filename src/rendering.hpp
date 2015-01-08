#ifndef INCLUDE_BRENDERING
#define INCLUDE_BRENDERING

#include "blib.hpp"

namespace Sys
{
    extern float view_x, view_y;
    extern bfont * afont;
    extern const SDL_Rect shape; // Can you have multiple extern declarations for the same symbol?
    bool RenderThings();
    bool PresentScreen();
}

#endif
