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
    
    SDL_Texture * check_texture_pile(std::string);
    
    bool FrameLimit();
    
    typedef enum
    {
        CHAIN_CONTINUE,
        CHAIN_FINISH
    } chainreturn;
    typedef chainreturn(*event_bubble)(SDL_Event);
    extern std::vector<event_bubble> event_chain;
    
    bool SDLEvents();
}

#endif
