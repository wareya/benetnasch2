#include "drawingcomponents.hpp"

#include "../bengine.hpp" // Sys::Renderer
#include "../blib.hpp" // loadTexture

namespace Sys
{
    BackgroundDrawable::BackgroundDrawable(entityid_t myEntity) : Component(myEntity), sprite(NULL)
    {
        position = new Position(myEntity, 0, 0);
        BackgroundDrawables.add(this);
    }
    BackgroundDrawable::~BackgroundDrawable()
    {
        delete position;
        SDL_DestroyTexture(sprite);
        
        BackgroundDrawables.remove(this);
    }
    bool BackgroundDrawable::init(const char * sarg)
    {
        sprite = loadTexture( sarg, Sys::Renderer );
        return sprite != nullptr;
    }
}
