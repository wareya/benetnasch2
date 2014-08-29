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
    bool inputs[256] = { }; 
    bool last_inputs[256] = { }; 
    signed char scrolls[4];
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
    
    void Init()
    {
        corestate = SDL_GetKeyboardState(NULL);
        keybindings.push_back({SDL_SCANCODE_E, JUMP});
        keybindings.push_back({SDL_SCANCODE_W, LEFT});
        keybindings.push_back({SDL_SCANCODE_D, DOWN});
        keybindings.push_back({SDL_SCANCODE_F, RIGHT});
        
        mousebindings.push_back({SDL_BUTTON_LEFT, SHOOT});
    }
    
    void Update()
    {
        auto mousebitmask = SDL_GetMouseState(&mx, &my);
        for (short i = 0; i < NUMBER_INPUTS; i++)
        {
            last_inputs[i] = inputs[i];
            inputs[i] = 0;
        }
        for (auto bind : keybindings)
        {
            if(corestate[bind.key])
                inputs[bind.input_index] = true;
        }
        for (auto bind : mousebindings)
        {
            if(mousebitmask & bind.button)
                inputs[bind.input_index] = true;
        }
    }
}
