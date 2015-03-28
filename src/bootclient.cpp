#include "blib.hpp"
#include "bengine.hpp"
#include "physics.hpp"
#include "maps.hpp"
#include "rendering.hpp"
#include "components/gamecomponents.hpp"
#include "components/player.hpp"
#include "input.hpp"
#include "network.hpp"
#include "netconst.hpp"
#include "client/clientdata.hpp"
#include "client/think.hpp"
#include "client/nethandlers.hpp"
#include "client/scripting.hpp"
#include "client/textinput.hpp"
#include "physics.hpp"
#include "serverplayer.hpp"
#include "components/textwindow.hpp"


// push systems into the mainloop
bool sys_init()
{
    #ifndef B_DEBUG_COREFRAMES
        Maps::load_wallmask("wallmask.png");
        Maps::load_background("background.png");
        
        Sys::myinput.Init();
        Sys::myself = nullptr;
        Sys::did_send_playerrequest = false;
        
        Sys::afont = new bfont(Sys::Renderer, std::string("The Strider.bmp"));
        ClientEngine::console.display = new Sys::TextWindow(Ent::New());
        auto t = ClientEngine::console.display;
        t->append_line("Test t");
        t->append_line("Test j");
        t->append_line("Test 1");
        t->append_line("Test 2");
        t->append_line("Test 3");
        t->append_line("Test 4");
        t->append_line("Test 5");
        ClientEngine::console.input = new Sys::TextBox(Ent::New());
        
        Sys::event_chain.push_back(&ClientEngine::Hotkeys); // client/textinput
    #endif
    
    Sys::tems.push_back(&Sys::FrameLimit); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::UpdateDelta); // physics
        Sys::tems.push_back(&Net::think);
    #endif
    Sys::tems.push_back(&Sys::SDLEvents); // bengine
    #ifndef B_DEBUG_COREFRAMES
        Sys::tems.push_back(&Sys::ClientThink); // client/think
        Sys::tems.push_back(&Sys::Physics); // physics
    #endif
    Sys::tems.push_back(&Sys::RenderThings); // rendering
    Sys::tems.push_back(&Lua::do_hud); // rendering
    Sys::tems.push_back(&Sys::PresentScreen); // rendering
    
    return 1; // remove sys_init from mainloop
}

bool main_init()
{
    // random display thing that should be moved elsewhere
	Sys::speeds.push_back(0);
	
	// initialize SDL
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;
    
    Sys::MainWindow = SDL_CreateWindow("Benetnasch", 300, 300, Sys::shape.w, Sys::shape.h, SDL_WINDOW_SHOWN);
    if (Sys::MainWindow == nullptr)
        std::cout << "Could not create an SDL window: " << SDL_GetError() << std::endl;
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_ACCELERATED|SDL_RENDERER_TARGETTEXTURE);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    // don't know if I actually use randomness anywhere
    srand(time(NULL));
    SDL_PumpEvents();
    
    
    // sets up hud and such
    Lua::scripting_init();
    
    // input reading structure
    Sys::myinput.Init();
    
    // load faucnet dll and assign function pointers
    if(faucnet_init() < 0)
        return 0;
    Net::setup_channels();
    
    // something for the renderers! yeah!
    Sys::view_x = 0;
    Sys::view_y = 0;
    
    // get mainloop ready
    Sys::tems.push_back(&sys_init);
    
    return 0;
}
