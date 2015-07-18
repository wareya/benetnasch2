#ifndef INCLUDE_BDRAWINGCOMPONENTS
#define INCLUDE_BDRAWINGCOMPONENTS

#include <SDL2/SDL.h>
#undef main

#include "../components.hpp"
#include "primitives.hpp"

namespace Sys
{
    // Sprite component
    // TODO: move sprite texture loading to a non-component structure
    struct TexturedDrawable : public Component
    {
        TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset);
        TexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, char noaddifdefined);
        ~TexturedDrawable();
        bool visible = true;
        Position * position;
        SDL_Texture * sprite;
        double xoffset, yoffset;
        bool flip = false;
        bool set_sprite(const char * sarg);
    };
    extern Collection<TexturedDrawable> TexturedDrawables;
    
    struct RotatingTexturedDrawable : public TexturedDrawable
    {
        // application: +position, +offset, ~rotation around origin
        // offset is still useful because position can be (and usually is) overwritten to point to another entity's position
        RotatingTexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, double angle, double xorigin, double yorigin);
        ~RotatingTexturedDrawable();
        double angle, xorigin, yorigin;
    };
    extern Collection<RotatingTexturedDrawable> RotatingTexturedDrawables;
    
    struct AnimatedTexturedDrawable : public TexturedDrawable
    {
        // application: frame size, number of frames, progress/index
        AnimatedTexturedDrawable(entityid_t myEntity, double argx, double argy, double argxoffset, double argyoffset, int length, double width, double height);
        ~AnimatedTexturedDrawable();
        int length;
        double index, width, height;
    };
    extern Collection<AnimatedTexturedDrawable> AnimatedTexturedDrawables;
    
    // background component
    struct BackgroundDrawable : public Component
    {
        BackgroundDrawable(entityid_t myEntity);
        ~BackgroundDrawable();
        Position * position;
        SDL_Texture * sprite;
        bool init(const char * sarg);
    };
    extern Collection<BackgroundDrawable> BackgroundDrawables;
    
}

#endif
