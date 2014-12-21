#include "input.hpp"
#include "blib/bmath.hpp"
#include "bengine.hpp"

#include <SDL2/SDL.h>
#undef main

#include <iostream>

namespace Input
{
    PlayerInput::PlayerInput()
    {
        // this is a constructor
        for (int i = 0; i < NUMBER_INPUTS; i++) // this is a for loop
        {
            inputs[i] = 0; // this is an initialization
            last_inputs[i] = 0;
        }
        aimDirection = 0;
        aimDistance = 0;
    }
    void PlayerInput::cycleInput() // this is a stateful method
    {
        for (short i = 0; i < NUMBER_INPUTS; i++)
        {
            last_inputs[i] = inputs[i]; // this is a state update
        }
    }
    unsigned short PlayerInput::getInputsAsBitfield()
    {
        unsigned short retvalue;
        unsigned short mask;
        for (int i = 0; i < NUMBER_INPUTS; i++)
        {
            mask = 1 << i;
            retvalue = retvalue | (inputs[i]?mask:0);
        }
        return retvalue;
    }
    // used in networking code
    void PlayerInput::setInputsAsBitfield(unsigned short invalue)
    {
        unsigned short mask;
        for (int i = 0; i < NUMBER_INPUTS; i++)
        {
            mask = 1 << i;
            inputs[i] = (invalue & mask)?1:0;
        }
    }
        
    void ClientInput::Init() // set up default key binds
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
        myplayerinput.cycleInput();
        // zero-out existing inputs
        for (int i = 0; i < NUMBER_INPUTS; i++)
            myplayerinput.inputs[i] = 0;
        // check key binds and apply appropriate inputs
        for (auto bind : keybindings)
        {
            if(corestate[bind.key])
                myplayerinput.inputs[bind.input_index] = true;
        }
        // check mouse buttons and apply appropriate inputs
        for (auto bind : mousebindings)
        {
            if(mousebitmask & bind.button)
                myplayerinput.inputs[bind.input_index] = true;
        }
        // store aim direction/distance in desired angle encoding and granularity (respectively)
        myplayerinput.aimDirection = fmod(point_direction(Sys::shape.w/2, Sys::shape.h/2, mx, my)+360.0, 360.0);
        myplayerinput.aimDistance = point_distance(Sys::shape.w/2, Sys::shape.h/2, mx, my);
    }
}
