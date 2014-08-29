#ifndef INCLUDE_B_PHYSICS_SUBROUTINES
#define INCLUDE_B_PHYSICS_SUBROUTINES

#include "../physics.hpp"
#include "../blib.hpp"
#include <tuple>

namespace Sys
{
    namespace Physicsers
    {
        /* place_meeting -- returns whether a Character overlaps any wallchunks */
        // TODO: make non-character-specific
        bool place_meeting (Character * character, float x, float y);
        /* place_meeting_which -- returns which wallchunks a Character overlaps */
        std::vector<BoxDrawable*> place_meeting_which (float x1, float y1, float x2, float y2);
        /* place_meeting_which -- returns which wallchunks a given line segment overlaps */
        std::vector<BoxDrawable*> line_meeting_which (float x1, float y1, float x2, float y2);

        /* move_contact */
        // moves our character to contact with a wall chunk (sends character backwards in cases of already overlapping)
        // returns square distance traveled
        // algorithm reference image: http://i.imgur.com/tq1rulr.png
        // TODO: make non-character-specific
        std::tuple<float, float> move_contact (Character * character, double hvec, double vvec);
    }
}

#endif
