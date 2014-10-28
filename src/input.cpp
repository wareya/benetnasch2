#include "input.hpp"
#include "blib/bmath.hpp"
#include "bengine.hpp"

#include <SDL2/SDL.h>
#undef main

namespace Input
{
    void ClientInput::Init()
    {
        corestate = SDL_GetKeyboardState(NULL);
        keybindings.push_back({SDL_SCANCODE_E, JUMP});
        keybindings.push_back({SDL_SCANCODE_W, LEFT});
        keybindings.push_back({SDL_SCANCODE_D, DOWN});
        keybindings.push_back({SDL_SCANCODE_F, RIGHT});
        
        mousebindings.push_back({SDL_BUTTON_LEFT, SHOOT});
    }
    
    void ClientInput::Update()
    {
        auto mousebitmask = SDL_GetMouseState(&mx, &my);
        for (short i = 0; i < NUMBER_INPUTS; i++)
        {
            myplayerinput.last_inputs[i] = myplayerinput.inputs[i];
            myplayerinput.inputs[i] = 0;
        }
        for (auto bind : keybindings)
        {
            if(corestate[bind.key])
                myplayerinput.inputs[bind.input_index] = true;
        }
        for (auto bind : mousebindings)
        {
            if(mousebitmask & bind.button)
                myplayerinput.inputs[bind.input_index] = true;
        }
        myplayerinput.aimDirection = fmod(point_direction(Sys::shape.w/2, Sys::shape.h/2, mx, my)+360.0, 360.0);
        myplayerinput.aimDistance = point_distance(Sys::shape.w/2, Sys::shape.h/2, mx, my);
    }
}
