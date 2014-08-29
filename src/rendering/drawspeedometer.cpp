#include "renderers.hpp"
#include "commonrenderdep.hpp"
#include "../speed.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawSpeedometer(float x, float y)
        {
        	int i = 0;
            for(auto speed : speeds)
            {
	            SDL_SetRenderDrawColor( Sys::Renderer,
	            						speed>0?255:0 + fmod(speed,100.0f),
	            						0,
	            						speed>0?0:255 + fmod(speed,100.0f), 255 );
	            
                SDL_RenderDrawLine(Sys::Renderer,
                				   Sys::shape.w-speeds.size()+i, Sys::shape.h-speed/3-150, Sys::shape.w-speeds.size()+i, Sys::shape.h-150);
                
	            SDL_SetRenderDrawColor( Sys::Renderer, 255, 0, fmod(speed,100.0f), 255 );
	            
                SDL_RenderDrawLine(Sys::Renderer,
                				   Sys::shape.w-speeds.size()+i, Sys::shape.h-abs(speed/3)-150, Sys::shape.w-speeds.size()+i, Sys::shape.h-150);
                
                i += 1;
            };
            
	        renderText(Sys::shape.w-100, Sys::shape.h-13,
	                   (std::string("speed: ")+std::to_string(speeds.back())).data(),
	                   Sys::afont);
	        
            return false;
        }
    }
}
