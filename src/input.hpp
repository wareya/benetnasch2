#ifndef INCLUDED_BINPUT
#define INCLUDED_BINPUT

#include <SDL2/SDL.h>
#undef main
#include <vector>

namespace Input
{
    enum {
        JUMP,
        LEFT,
        RIGHT,
        DOWN,
        SHOOT,
        NUMBER_INPUTS
    };
    extern bool inputs[256];
    extern bool last_inputs[256];
    extern signed char scrolls[4];
    extern int mx;
    extern int my;
    extern const Uint8 * corestate;
    
    struct keybinding
    {
        SDL_Scancode key;
        unsigned char input_index;
    };
    extern std::vector<keybinding> keybindings;
    struct mousebinding
    {
        uint32_t button;
        unsigned char input_index;
    };
    extern std::vector<mousebinding> mousebindings;
    
    void Init();
    
    void Update();
}

#endif
