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
        double TimeWaitms;
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
           	TimeWaitms = Time::Frametime;
            #else
            TimeWaitms = maximum(0.0, Time::Frametime - TimeSpent/1000.0 - Time::deviance);
            #endif
            // store time for delay deviance check
            prehalttime = Time::get_us();
            
        	#ifndef B_FRAMELIMIT_DISABLE
            SDL_Delay(round(TimeWaitms));
	        #endif
            
            // measure current time directly after frame limit delay
            halttime = Time::get_us();
            
            Time::error = (halttime - prehalttime)/1000.0 - round(TimeWaitms);
            // calculate amount of time that halt was inaccurate by
            // and store it in order to to compensate for it in the next iteration
            Time::deviance = (halttime - prehalttime)/1000.0 - TimeWaitms;
            // allow framelimiter to make up for single frames of poor performance
            if(TimeSpent/1000.0 > Time::Frametime)
                Time::deviance += TimeSpent/1000.0 - Time::Frametime;
            
            // incrememt tick count
            Time::ticks = fmod(Time::ticks + 1.0, Time::Framerate);
        }
        // push timings to buffer
        Time::last_us = Time::simstart_us;
        Time::simstart_us = halttime;
        Time::frames.push_front( Time::simstart_us );
        Time::delta_us = ( (Time::frames[0] - Time::frames[1])
                         + (Time::frames[1] - Time::frames[2]) 
                         )/2;
        Time::delta = Time::delta_us / Time::scale;
        Time::sim = TimeSpent;
        Time::halt = halttime-prehalttime;
        Time::asked = round(TimeWaitms);
        
        // Throw away old timings
        while (Time::frames.size() > 50)
            Time::frames.erase( Time::frames.end() );
        
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
