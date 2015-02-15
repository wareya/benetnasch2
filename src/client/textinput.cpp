#include "textinput.hpp"
#include "../components/textwindow.hpp"

#include <string.h>

namespace ClientEngine
{
    Console console;
    bool consoleActive = 0;
    
    Sys::chainreturn Hotkeys(SDL_Event event)
    {
        switch(event.type)
        {
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_BACKQUOTE:
                consoleActive = !consoleActive;
                console.display->visible = consoleActive;
                console.input->visible   = consoleActive;
                if(consoleActive)
                {
                    SDL_StartTextInput();
                    console.input->line = "";
                }
                else
                {
                    SDL_StopTextInput();
                    console.input->line = "";
                }
                return Sys::CHAIN_FINISH;
            case SDLK_RETURN:
                consoleActive = false;
                console.display->visible = consoleActive;
                console.input->visible   = consoleActive;
                SDL_StopTextInput();
                console.input->line = "";
                // TODO: Commit command
                return Sys::CHAIN_FINISH;
            case SDLK_BACKSPACE:
                console.input->line.erase(console.input->line.end()-1);
            }
            break;
        case SDL_TEXTINPUT:
            if(strcmp("`", event.text.text) != 0)
                console.input->line += event.text.text;
            break;
        }
        return Sys::CHAIN_CONTINUE;
    }
    
    Sys::chainreturn TextInput(SDL_Event event)
    {
        switch(event.type)
        {
        case 0:
            break;
        }
        return Sys::CHAIN_CONTINUE;
    }
}
