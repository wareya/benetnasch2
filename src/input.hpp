#ifndef INCLUDE_INPUT
#define INCLUDE_INPUT

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
    struct PlayerInput
    {
        PlayerInput();
        
        bool inputs[NUMBER_INPUTS] = { };
        bool last_inputs[NUMBER_INPUTS] = { };
        double aimDirection;
        double aimDistance;
        
        unsigned short getInputsAsBitfield();
        void cycleInput();
        void setInputsAsBitfield(unsigned short invalue);
    };
    struct ClientInput
    {
        PlayerInput myplayerinput;
        int mx;
        int my;
        const Uint8 * corestate;
        
        struct keybinding
        {
            SDL_Scancode key;
            unsigned char input_index;
        };
        std::vector<keybinding> keybindings;
        struct mousebinding
        {
            uint32_t button;
            unsigned char input_index;
        };
        std::vector<mousebinding> mousebindings;
        
        void Init();
        void Update();
    };
}

#endif
