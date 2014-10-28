#include "bengine.hpp"
#include "blib.hpp"
#include "physics.hpp"
#include "input.hpp"

namespace Sys
{
    SDL_Window * MainWindow;
    SDL_Renderer * Renderer;
    const SDL_Rect shape = {0, 0, 800, 600};
    bool quit = false;
    
    std::vector<bool(*)()> tems;
    
    nall::map<std::string, SDL_Texture *> TexturePile;
    
    
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
            }   
        }
        return false;
    }
}
