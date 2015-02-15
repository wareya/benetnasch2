#ifndef TEXTINPUT
#define TEXTINPUT

#include "../bengine.hpp"
#include "../components/textwindow.hpp"

namespace ClientEngine
{
    struct Console
    {
        Sys::TextWindow * display;
        Sys::TextBox * input;
    };
    
    extern Console console;
    
    Sys::chainreturn Hotkeys(SDL_Event);
    Sys::chainreturn TextInput(SDL_Event);
}

#endif
