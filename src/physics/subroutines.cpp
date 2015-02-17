#include "subroutines.hpp"

namespace Sys
{
    namespace Physicsers
    {
        /* place_meeting -- returns whether a Character overlaps any wallchunks */
        // TODO: make non-character-specific
        bool place_meeting (Character * character, float x, float y)
        {
            bool collided = false;
            for(auto wallchunk : Sys::BoxDrawables)
            {
                if(aabb_overlap(wallchunk->position->x,                      wallchunk->position->y,
                                wallchunk->position->x + wallchunk->hull->w, wallchunk->position->y + wallchunk->hull->h,
                                character->position->x + x + character->hull->xoffset,
                                character->position->y + y + character->hull->yoffset,
                                character->position->x + x + character->hull->xoffset + character->hull->w,
                                character->position->y + y + character->hull->yoffset + character->hull->h))
                {
                    collided = true;
                    break;
                }
            }
            return collided;
        }
        /* place_meeting_which -- returns which wallchunks a Character overlaps */
        std::vector<BoxDrawable*> place_meeting_which (float x1, float y1, float x2, float y2)
        {
            std::vector<BoxDrawable*> overlaps;
            for(auto wallchunk : Sys::BoxDrawables)
            {
                if(aabb_overlap(wallchunk->position->x                   , wallchunk->position->y                   ,
                                wallchunk->position->x+wallchunk->hull->w, wallchunk->position->y+wallchunk->hull->h,
                                x1, y1,
                                x2, y2))
                {
                    overlaps.push_back(wallchunk);
                }
            }
            return overlaps;
        }
        /* place_meeting_which -- returns which wallchunks a given line segment overlaps */
        std::vector<BoxDrawable*> line_meeting_which (float x1, float y1, float x2, float y2)
        {
            std::vector<BoxDrawable*> overlaps;
            for(auto wallchunk : place_meeting_which(x1, y1, x2, y2))
            {
                if(line_aabb_overlap(x1, y1, x2, y2,
                                     wallchunk->position->x                   , wallchunk->position->y,
                                     wallchunk->position->x+wallchunk->hull->w, wallchunk->position->y+wallchunk->hull->h
                                     ))
                {
                    overlaps.push_back(wallchunk);
                }
            }
            return overlaps;
        }

        /* move_contact */
        // moves our character to contact with a wall chunk (sends character backwards in cases of already overlapping)
        // returns square distance traveled
        // algorithm reference image: http://i.imgur.com/tq1rulr.png
        // TODO: make non-character-specific
        std::tuple<float, float> move_contact (Character * character, double hvec, double vvec)
        {
            if(hvec == 0 and vvec == 0)
            {
                //puts("empty move_contact call");
                return std::tuple<float, float>(0.0f, 0.0f);
            }
            double &width = character->hull->w;
            double &height = character->hull->h;
            
            // will be written temporarily
            auto x = character->position->x + character->hull->xoffset;
            auto y = character->position->y + character->hull->yoffset;
            
            float xsign = ssign(hvec);
            float ysign = ssign(vvec);
            //printf("input: %f %f %f %f %f %f\n", x, y, width, height, hvec, vvec);
            
            /* PART ONE: make a motion bounding rect and include any collision rect that overlaps it */
            
            auto which = place_meeting_which(x+(xsign < 0 ? width : 0)     , y+(ysign < 0 ? height : 0)     ,
                                             x+(xsign > 0 ? width : 0)+hvec, y+(ysign > 0 ? height : 0)+vvec);
            //std::cout << which.size() << "\n";
            
            /* PART TWO: ignore cases where we couldn't possibly collide with anything */
            
            if(which.size() == 0)
            {
                x += hvec;
                y += vvec;
                //puts("no collision at move_contact");
                return std::tuple<float, float>(hvec, vvec);
            }
            /* PART THREE: Figure out which things we could collide with, and which one is closest */
            else
            {
                /* Consult the picture at the top of the function from hereon out */
                auto oldx = x;
                auto oldy = y;
                
                auto red_x = x + hvec + (xsign > 0 ? width  : 0);
                auto red_y = y + vvec + (ysign > 0 ? height : 0);
                
                float furthest = 0;
                float rx;
                float ry;
                // loop through each box to eject from and pick the one with the furthest ejection
                /* ( Pick the closest box along the line of movement ) */
                for(auto box : which) // this WILL run at least once
                {
                    auto eject_x = x;
                    auto eject_y = y;
                    
                    // super cool important things that are used for everything
                    auto green_x = box->position->x + (xsign > 0 ? 0 : box->hull->w);
                    auto green_y = box->position->y + (ysign > 0 ? 0 : box->hull->h);
                    
                    auto inner_height = red_y - green_y;
                    auto inner_width  = red_x - green_x;
                    
                    float blue_x, blue_y;
                    if(hvec != 0)
                    {
                        blue_y = red_y - (inner_width/hvec)*vvec;
                        if(ssign(blue_y - green_y) == ysign)
                        {
                            blue_x = green_x;
                            goto tail; // fuck you this is legit
                        }
                    }
                    // falls through to here if the blue dot is on the wrong side of green line
                    blue_x = red_x - (inner_height/vvec)*hvec;
                    blue_y = green_y;
                    
                    tail:
                    eject_x = blue_x - (xsign > 0 ? width  : 0);
                    eject_y = blue_y - (ysign > 0 ? height : 0);
                    auto eject_sqdist = sqdist(blue_x - red_x, blue_y - red_y);
                    if(eject_sqdist > furthest)
                    {
                        furthest = eject_sqdist;
                        rx = eject_x;
                        ry = eject_y;
                    }
                }
                x = rx;
                y = ry;
                
                character->position->x = x - character->hull->xoffset;
                character->position->y = y - character->hull->yoffset;
                
                return std::tuple<float, float>(x - oldx, y - oldy);
            }
        }
    }
}
