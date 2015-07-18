#include "subroutines.hpp"
#include "../input.hpp"
#include "../components/player.hpp"
#include <vector>

namespace Sys
{
    namespace Physicsers
    {
        bool MoveCharacters()
        {
            if(!Physicsers::delta_is_too_damn_low)
            {
                for(auto player : Sys::Players)
                {
                    auto character = player->character;
                    if(!character)
                        continue;
                    /*
                     *  predef
                     */
                    double &x = character->position->x;
                    double &y = character->position->y;
                    double &hspeed = character->hspeed;
                    double &vspeed = character->vspeed;
                    
                    
                    int stepsize = 4;
                    
                    /*
                     *  handle accelerations
                     */
                    float accel = 1000*delta;
                    float aaccel = 1000*delta;
                    float gravity = 720*delta;
                    float max_gravity = 2000;
                    float jumpspeed = -300;
                    float fric_overmoving = pow(0.4, delta);
                    float fric_moving = pow(0.2, delta);
                    float fric_counter = pow(0.01, delta);
                    float fric_still = pow(0.025, delta);
                    float fric_sticky = 100*delta;
                    float fric_air = pow(0.15, delta);
                    float fric_air_threshhold = 400;
                    int crawlspeed = 75;
                    int walkspeed = 170;
                    int runspeed = 240;
                    int airrunspeed = 260;
                    float struggle1 = 0.65;
                    float struggle2 = 0.5;
                    
                    auto & input = player->input;
                    int direction = (input.inputs[Input::RIGHT] - input.inputs[Input::LEFT]);
                    int jumping = (input.inputs[Input::JUMP] & !input.last_inputs[Input::JUMP]);
                    
                    // update weapon things
                    auto rawangle = input.aimDirection;
                    auto dir = deg2rad(rawangle);
                    
                    int shooting = (input.inputs[Input::SHOOT] and not input.last_inputs[Input::SHOOT]);
                    if(shooting)
                    {
                        auto shotspeed = 800;
                        
                        new Bullet(Ent::New(), character->center_x(), character->center_y(), cos(dir) * shotspeed + hspeed, -sin(dir) * shotspeed, 1);
                    }
                    
                    int whichrunspeed;
                    int taccel;
                    if(!place_meeting(character, 0, crop1(gravity))) // in air
                    {
                        taccel = aaccel;
                        whichrunspeed = airrunspeed;
                    }
                    else
                    {
                        taccel = accel;
                        whichrunspeed = runspeed;
                    }
                    // If we're moving too slowly, our acceleration should be dampened
                    if (direction == int(sign(hspeed)) or hspeed == 0)
                    {
                        if (abs(hspeed) >= walkspeed)
                            taccel *= struggle2;
                        else if (abs(hspeed) >= crawlspeed)
                            ;
                        else
                        {
                            float factor =
                              (float(abs(hspeed)) - crawlspeed)
                              / walkspeed;
                            taccel *= lerp(struggle1, 1.0f, factor);
                        }
                    }
                    
                    // calculate post-control speed
                    
                    auto hsign = (0 < hspeed) - (hspeed < 0);
                    
                    auto started_in_excess = abs(hspeed) > whichrunspeed and direction == hsign;
                    
                    if(!started_in_excess)
                    {
                        float walk_solution;
                        if(abs(hspeed) < crawlspeed) // no friction if extremely slow
                            walk_solution = hspeed + taccel*direction;
                        else // friction if we're extremely fast
                            walk_solution = (hspeed + taccel*direction) * fric_moving;
                        
                        // If we're changing directions, we do either friction OR deacceleration; whichever one is stronger
                        // this is the counter-friction
                        auto fric_solution = hspeed * fric_counter; // same sign
                        
                        if(direction < 0) // prefer more negative
                            hspeed = minimum(walk_solution, fric_solution);
                        if(direction > 0) // more positive
                            hspeed = maximum(walk_solution, fric_solution);
                            
                        hsign = (0 < hspeed) - (hspeed < 0);
                        
                        if (abs(hspeed) > whichrunspeed and started_in_excess)
                            hspeed = hsign * whichrunspeed;
                    }
                    else // did start in excess
                    {
                        if(place_meeting(character, 0, crop1(gravity))) // run friction if on ground
                        {
                            hspeed *= fric_overmoving;
                        }
                        if(abs(hspeed) < whichrunspeed and direction == hsign) // set speed to max if friction set us below our desired speed
                            hspeed = whichrunspeed*hsign;
                    }
                    
                    if (direction == 0)
                    {
                        hspeed *= fric_still;
                        hspeed = absolute(hspeed) - fric_sticky;
                        hspeed = hspeed > 0 ? hspeed : 0;
                        hspeed *= hsign;
                    }
                    
                    if(!place_meeting(character, 0, crop1(gravity)))
                    {
                        vspeed += gravity;
                        if(vspeed > max_gravity)
                            vspeed = max_gravity;
                    }
                    if(jumping)
                        vspeed = jumpspeed;
                    auto abv = fabs(vspeed);
                    if(abv > fric_air_threshhold)
                    {
                        abv = (abv-fric_air_threshhold)*fric_air;
                        vspeed = sign(vspeed)*(abv+fric_air_threshhold);
                    }
                    
                    /*
                     *  muh movement solving
                     */
                    
                    hspeed *= delta;
                    vspeed *= delta;
                    auto h_auto = hspeed;
                    auto v_auto = vspeed;
                    
                    /* movement solver starts here */
                    // we're in the wallmask; try to get out of it if it's really easy
                    if (place_meeting(character, 0, 0))
                    {
                        //puts("woaaAAAHHHh we're in the wallmask 1!");
                        for (int i = 1; i < stepsize; i += 1)
                        {
                            if(!place_meeting(character, 0, -i))
                            {
                                y -= i;
                                //puts("eject up");
                                break;
                            }
                        }
                    }
                    int max_bounces = 3;
                    for (int i = max_bounces; i > 0; --i)
                    {
                        if (place_meeting(character, hspeed, vspeed)) // we collided with something
                        {
                            //puts("rectifying a collision");
                            float mx, my;
                            // move snugly to whatever we might've hit, and store the x and y deltas from that motion
                            std::tie(mx, my) = move_contact(character, hspeed, vspeed);
                            //std::cout << "move_contact-ed " << vector_length(vspeed, hspeed) << " to " << vector_length(mx, my) << "\n";
                            
                            // subtract the motion we've already done this from from this frame's automatic motion
                            h_auto -= mx;
                            v_auto -= my;
                            
                            if(sqdist(mx, my) != sqdist(vspeed, hspeed)) // avoid obscure spatial-temporal aliasing bug (we hit a wall *perfectly* snug)
                            {
                                // check whether we contacted walls/slopes or not
                                if(place_meeting(character, crop1(h_auto), 0))
                                {
                                    // store original y before sloping
                                    auto oy = y;
                                    // check for slopes
                                    for (int i = stepsize; i <= abs(h_auto)+stepsize; i += stepsize) // don't climb stairs that are too steep, even if we're moving fast; handle appropriately steep slopes appropriately without requiring more bounces -- this might actually have a problem the way it currently is, I might rewrite it
                                    {
                                        //puts("testing slopes");
                                        if(!place_meeting(character, h_auto, i)) // slope down a sloped ceiling step
                                        {
                                            y += i;
                                            //puts("downceil");
                                            break;
                                        }
                                        else if(!place_meeting(character, h_auto, -i)) // slope up a normal ground slope
                                        {
                                            y -= i;
                                            //puts("upslope");
                                            break;
                                        }
                                    }
                                    // no slope; it's a wall
                                    if(oy == y)
                                    {
                                        //puts("w");
                                        hspeed = 0;
                                        h_auto = 0;
                                    }
                                }
                                // assume floor otherwise
                                else
                                {
                                    //puts("f");
                                    vspeed = 0;
                                    v_auto = 0;
                                }
                            }
                            else
                                break;
                        }
                        // we did not collide with something
                        else
                        {
                            // we might want to "down" a slope
                            bool sloped = false;
                            // only if we're walking on the ground
                            if(vspeed == 0)
                            {
                                for (int i = stepsize; i <= abs(h_auto)+stepsize; i += stepsize)
                                {
                                    if(!place_meeting(character, h_auto, i) and place_meeting(character, h_auto, i+1))
                                    {
                                        sloped = true;
                                        //puts("downslope");
                                        y += i;
                                        vspeed = 0;
                                        v_auto = 0;
                                        break;
                                    }
                                }
                            }
                            if(!sloped) // whole bounce with no collisions to do
                                break;
                        }
                        if(v_auto == 0 and h_auto == 0) // exhausted distance to travel anyway
                            break;
                    }
                    
                    x += h_auto;
                    y += v_auto;
                    //if (place_meeting(character, 0, 0))
                        //puts("woaaAAAHHHh we're in the wallmask 2!");
                    
                    hspeed /= delta;
                    vspeed /= delta;
                    //puts("end frame");
                    
                    #ifdef CLIENT
                        auto aiming_left = (rawangle >= 90 and rawangle < 270);
                        
                        character->weaponsprite->angle = rawangle-180*aiming_left;
                        character->weaponsprite->flip = aiming_left;
                        character->stand->flip = aiming_left;
                        character->run->flip = aiming_left;
                        
                        auto running = (fabs(hspeed) > crawlspeed);
                        character->stand->visible = !running;
                        character->run->visible = running;
                        if(running)
                            character->run->index += hspeed/35*delta*(character->run->flip*-2+1);
                        else
                            character->run->index = 0;
                    #endif
                    
                    player->input.cycleInput();
                }
            }
            return false;
        }
    }
}
