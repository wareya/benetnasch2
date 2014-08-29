#ifndef INCLUDE_BPRIMITIVES
#define INCLUDE_BPRIMITIVES

#include "../components.hpp"

namespace Sys
{
    // Position component
    struct Position : public Component
    {
        Position(entityid_t myEntity, double argx, double argy);
        ~Position();
        double x, y;
    };
    // collection of positions
    extern Collection<Position> Positions;
    
    // Hull component
    struct Hull : public Component
    {
        Hull(entityid_t myEntity, double argw, double argh, double argxoffset, double argyoffset);
        ~Hull();
        double w, h, xoffset, yoffset;
    };
    extern Collection<Hull> Hulls;
}

#endif
