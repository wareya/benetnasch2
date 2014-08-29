#ifndef INCLUDE_BRENDERING
#define INCLUDE_BRENDERING

#include "blib.hpp"

namespace Sys
{
    extern float view_x, view_y;
    extern bfont * afont;
    bool RenderThings();
    bool PresentScreen();
}

#endif
