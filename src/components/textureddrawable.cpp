#include "drawingcomponents.hpp"

// TODO: move Sys::Renderer and TexturePile outside of the main engine ("bengine")

#include "../bengine.hpp" // TexturePile, Sys::Renderer
#include "../blib.hpp"

namespace Sys
{
    TexturedDrawable::TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset ) : Component(myEntity), sprite(NULL), xoffset(argxoffset), yoffset(argyoffset)
    {
        position = new Position(myEntity, argx, argy);
        TexturedDrawables.add(this);
    }
    TexturedDrawable::TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, char noaddifdefined ) : Component(myEntity), sprite(NULL), xoffset(argxoffset), yoffset(argyoffset)
    {
        position = new Position(myEntity, argx, argy);
    }
    TexturedDrawable::~TexturedDrawable()
    {
        if(position->entityID == entityID) delete position;
        
        TexturedDrawables.remove(this);
    }
    bool TexturedDrawable::set_sprite(const char * sarg)
    {
        auto t = TexturePile.find( std::string(sarg) );
        if(!t.valid) // null/not found
        {
            puts("Nonfound texture, loading");
            sprite = loadTexture( sarg, Sys::Renderer );
            if (sprite)
                TexturePile.insert(std::string(sarg), sprite);
        }
        else // already loaded
        {
            puts("Attempting to assign texture");
            sprite = *t.value;
        }
        std::cout << sprite << "\n";
        return sprite != nullptr;
    }
}
