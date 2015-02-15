#include "bengine.hpp"
#include "blib.hpp"
#include "physics.hpp"
#include "input.hpp"

namespace Sys
{
    // self-explanatory
    SDL_Window * MainWindow;
    // read the SDL documentation
    SDL_Renderer * Renderer;
    // do it
    const SDL_Rect shape = {0, 0, 800, 600};
    // whether we want to quit out of the engine
    bool quit = false;
    
    // list of Sys::tems -- basically just parts of the mainloop
    std::vector<bool(*)()> tems;
    
    // Texture storage map. Nice and simple, right?
    // Sorry for using an obscure library.
    nall::map<std::string, SDL_Texture *> TexturePile;
    
    SDL_Texture * check_texture_pile(std::string sarg)
    {
        auto t = TexturePile.find( std::string(sarg) );
        std::cout << "Looking for texture " << sarg;
        if(!t.valid) // null/not found
        {
            puts("Nonfound texture, loading");
            auto sprite = loadTexture( sarg, Sys::Renderer );
            if (sprite)
                TexturePile.insert(std::string(sarg), sprite);
            return sprite;
        }
        else // already loaded
        {
            puts("Found texture");
            return *t.value;
        }
    }
    
    bool FrameLimit()
    {
        // Frame limit
        double TimeWaitS;
        double TimeSpent;
        double prehalttime = Time::get_us(); // naively avoid error in non-critical debug code
        double halttime;
        
        if( Time::dostart )
        {
            SDL_Delay(1000.0/Time::Framerate);
            TimeSpent = 0;
            Time::dostart = false;
            Time::deviance = 0;
            halttime = Time::get_us();
        }
        else
        {
            // get time before halting
            // check time between end of last frame limit delay and current code
            TimeSpent = Time::get_us() - Time::simstart_us;
            // calculate desired amount to wait
        	#ifdef B_FRAMELIMIT_CHEAP
           	TimeWaitS = 1000.0 / Time::Framerate;
            #else
            TimeWaitS = maximum(0.0, (1000.0 / Time::Framerate) - TimeSpent/1000.0 - Time::deviance);
            #endif
            // store time for delay deviance check
            prehalttime = Time::get_us();
            
        	#ifndef B_FRAMELIMIT_DISABLE
	        SDL_Delay(round(TimeWaitS));
	        #endif
            
            // measure current time directly after frame limit delay
            halttime = Time::get_us();
            
            Time::error = (halttime - prehalttime)/1000.0 - round(TimeWaitS);
            // calculate amount of time that halt was inaccurate by and store it for the next iteration to compensate for
            Time::deviance = (halttime - prehalttime)/1000.0 - TimeWaitS;
            
            // incrememt tick count
            Time::ticks = fmod(Time::ticks + 1.0, Time::Framerate);
        }
        // push timings to buffer
        Time::last_us = Time::simstart_us;
        Time::simstart_us = halttime;
        Time::delta_us = Time::simstart_us - Time::last_us;
        Time::delta = Time::delta_us / Time::scale;
        Time::frames.push_back( Time::simstart_us );
        Time::sim = TimeSpent;
        Time::halt = halttime-prehalttime;
        
        // Throw away old timings
        while (Time::frames.size() > 125)
            Time::frames.erase( Time::frames.begin() );
        
        return false;
    }
    
    
    std::vector<event_bubble> event_chain;
    
    bool SDLEvents()
    {
        SDL_PumpEvents();
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            switch ( event.type )
            {
            case SDL_QUIT:
                quit = true;
                break;
            default:
                for ( auto func : event_chain )
                {
                    if( func(event) == CHAIN_FINISH )
                        break;
                }
                break;
            }   
        }
        return false;
    }
}
