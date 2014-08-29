#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional>
#include <cmath>

#include <nall/set.hpp>
#include <nall/map.hpp>

#include "blib.hpp"
#include "bengine.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "physics.hpp"
#include "speed.hpp"
#include "input.hpp"

bool sys_init()
{
    Sys::afont = new bfont(Sys::Renderer, std::string("The Strider.bmp"));
    
    #ifndef B_DEBUG_COREFRAMES
    Maps::load_wallmask("wallmask.png");
    Maps::load_background("background.png");
    
    auto me = new Sys::Character(Ent::New(), Maps::width/2, Maps::height/2);
    me->myself = true;
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
    #endif
    Sys::tems.push_back(&Sys::SDLEvents); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::Physics); // physics
    #endif
    Sys::tems.push_back(&Sys::RenderThings); // rendering
    Sys::tems.push_back(&Sys::PresentScreen); // rendering
    
    return 1;
}

bool main_init()
{
	speeds.push_back(0);
	
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;

    Sys::view_x = 0;
    Sys::view_y = 0;

    Sys::MainWindow = SDL_CreateWindow("Benetnasch", 300, 300, Sys::shape.w, Sys::shape.h, SDL_WINDOW_SHOWN);
    if (Sys::MainWindow == nullptr)
        std::cout << "Could not create an SDL window: " << SDL_GetError() << std::endl;
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_ACCELERATED);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    srand(time(NULL));
    
    SDL_PumpEvents();
    Input::Init();
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
#ifdef TESTS
int main(int argc, char *argv[])
{
    std::cout << "Testing line-aabb collision math...\n";
    std::cout << "Test1: " << line_aabb_overlap(0, 0, 12, 12, 4, 4, 8, 8) << "\n";
}
#else // not TESTS
int main(int argc, char *argv[])
{
    if (main_init())
        return 1;
    
    while ( !Sys::quit )
    {
        std::vector<bool(*)()> marked_for_removal;
        for(auto function : Sys::tems)
        {
            auto r = function();
            if(r)
                marked_for_removal.push_back(function);
        }
        for(auto function : marked_for_removal)
        {
            unsigned i;
            for (i = 0; Sys::tems[i] != function and i < Sys::tems.size(); ++i) ;
            if(i == Sys::tems.size())
                std::cout << "Bad system removal in mainloop!";
            else
                Sys::tems.erase(Sys::tems.begin()+i);
        }
    }
    return 0;
}
#endif // TESTS
