#include "renderers.hpp"
#include "commonrenderdep.hpp"
#include "../input.hpp"
#include "../physics.hpp"
#include "../client/clientdata.hpp"

namespace Sys
{
    namespace Renderers
    {
        bool DrawScreenText(float x, float y)
        {
            //sprintf("%d", );
            renderText(0, 0,
                       (std::string("FPS:  ")+std::to_string(Time::scale / ((Time::frames.back() - Time::frames.front())/(Time::frames.size()-1)))).data(),
                       Sys::afont);
        	#ifndef B_DEBUG_FRAMESONLY
            renderText(0, 13*1,
                       (std::string("Sim:  ")+std::to_string(Time::sim / 1000)).data(),
                       Sys::afont);
            renderText(0, 13*2,
                       (std::string("Halt: ")+std::to_string(Time::halt / 1000)).data(),
                       Sys::afont);
            renderText(0, 13*3,
                       (std::string("Dev:  ")+std::to_string(Time::deviance)).data(),
                       Sys::afont);
            renderText(0, 13*4,
                       (std::string("Chunks:")+std::to_string(Sys::BoxDrawables.List.size())).data(),
                       Sys::afont);
            renderText(0, 13*5,
                       (std::string("Bullets:")+std::to_string(Sys::Bullets.List.size())).data(),
                       Sys::afont);
            for(auto c : Sys::Characters)
            {
                if (c->myself) renderText(0, 13*6,
		                   (std::string("x, y:   ")+std::to_string(c->position->x)+std::string(" ")+std::to_string(c->position->y)).data(),
		                   Sys::afont);
		        break;
            }
            renderText(0, 13*7,
                       (std::string("Delta:  ")+std::to_string(Physicsers::delta)).data(),
                       Sys::afont);
            std::string inputstr("");
            if(myself)
            {
                for(short i = 0; i < Input::NUMBER_INPUTS; i++)
                    inputstr += std::to_string(myself->input.inputs[i]);
                renderText(0, 13*7,
                           (std::string("Inputs: ")+inputstr).data(),
                           Sys::afont);
            }
        	#endif
            return false;
        }
    }
}
