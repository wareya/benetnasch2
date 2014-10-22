#ifndef INCLUDE_BENGINE
#define INCLUDE_BENGINE

#include <SDL2/SDL.h>
#undef main
#include <vector>
#include <nall/map.hpp>

namespace Sys
{
    extern SDL_Window * MainWindow;
    extern SDL_Renderer * Renderer;
    extern const SDL_Rect shape;
    extern bool quit;
    
    extern std::vector<bool(*)()> tems; // Sys::tems
    
    extern nall::map<std::string, SDL_Texture *> TexturePile;
    
    bool FrameLimit();
    bool SDLEvents();
}

#endif
