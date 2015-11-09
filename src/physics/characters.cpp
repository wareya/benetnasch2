#include "subroutines.hpp"
#include "../input.hpp"
#include "../components/player.hpp"
#include <vector>

#ifdef CLIENT
#include "../samples.hpp"
#endif

static const float basegravity = 720;
static const float max_gravity = 2000;
static const float maxspeed = 300;


namespace Sys
{
    namespace Physicsers
    {
        // Benetnasch predicts movement and rescales it based on the actual time taken
        // in order to compromise on the advantages of both delta time and interpolated timestep.
        float given_movement(double hspeed, Input::PlayerInput input, Sys::Character * character, float delta, int * info)
        {
            /* Due to REASONS, Benetnasch isn't using the typical += accel *= friction method of handling walking speed.
             * Instead, we're going to "manually" model struggle as we approach the desired max speed.
             */
            // our values
            float baseaccel = 1000*delta;
            float deaccel = 1300*delta;
            float struggleslow = 0.2f; // struggle reaches 0.2 at low end
            float strugglehard = 0.15f; // struggle reaches 0.15 at high end
            float strugglepoint = 0.25f; // The point on the acceleration curve with the LEAST struggle
            
            int direction = (input.inputs[Input::RIGHT] - input.inputs[Input::LEFT]);
            
            // whether we're accelerating in the same direction we're moving
            // (with "not" for not controlling)
            int direction_agreement = (sign(direction) == sign(hspeed) and direction) != 0 ? 1 : -1;
            // with "yes" for not moving, yet controlling
            if (hspeed == 0 and direction) direction_agreement = 1;
            
            if(direction_agreement == 1)
            {
                float speed = fabs(hspeed);
                // We need to get how close we are to the max speed to model struggle
                float fraction = speed/maxspeed;
                bool started_lesser = (fraction < 1.0f);
                
                //hspeed += direction * baseaccel;
                
                
                if(started_lesser)
                {
                    // Transform how close we are to the max speed, into the modifier
                    if (fraction < strugglepoint)
                    {
                        fraction /= strugglepoint; // gradient 0 ~ 1
                        
                        fraction *= 1.0f-struggleslow; // gradient ex. 0 ~ 0.8
                        fraction += struggleslow; // gradient ex. 0.2 ~ 1
                    }
                    else
                    {
                        fraction -= strugglepoint; // gradient ex. 0.75 ~ 0
                        fraction /= 1.0f-strugglepoint; //gradient ex. 1.0 ~ 0
                        fraction *= -1; // gradient ex. -1.0 ~ 0
                        fraction += 1; // gradient ex. 0 ~ 1.0
                        
                        fraction *= fraction; // curve down
                        
                        fraction *= 1.0f-strugglehard; // gradient ex. 0 ~ 0.85
                        fraction += strugglehard; // gradient ex. 0.15 ~ 1
                        
                    }
                    
                    //fraction /= 1.0f-strugglemodel;
                    
                    speed += baseaccel*fraction;
                    #define INFO_CHECKFAST (1<<0)
                    *info |= INFO_CHECKFAST;
                    hspeed = direction * speed;
                }
                else
                {   // if we're agreeing with our movement, but STARTED going too fast,
                    // we need to use a completely different model in order to
                    // agree with the non-direction_agreement case.
                    // However, it still needs to be here (rather than non-direction_agreement)
                    // in order to gracefully go to maxspeed.
                    speed -= deaccel;
                    #define INFO_CHECKSLOW (1<<1)
                    *info |= INFO_CHECKSLOW;
                    hspeed = sign(hspeed) * speed;
                }
            }
            else // non-direction_agreement case
            {
                float speed = fabs(hspeed);
                
                float apply = deaccel;
                if(direction == 0)
                {   // Model struggle of approaching zero speed (rather than continuing to counter-accelerate) if we're not controlling
                    float fraction = speed/maxspeed;
                    if(fraction > 1.0)
                        fraction = 1.0;
                    
                    fraction *= 1.0f-struggleslow; // gradient ex. 0 ~ 0.8
                    fraction += struggleslow; // gradient ex. 0.2 ~ 1
                    
                    apply *= fraction;
                }
                
                #define INFO_CHECKREVERSE (1<<2)
                if(fabs(hspeed) < deaccel and direction == 0)
                    *info |= INFO_CHECKREVERSE;
                hspeed -= sign(hspeed)*apply;
            }
            
            return hspeed;
        }
        
        
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
                    
                    // update weapon things
                    Input::PlayerInput & input = player->input;
                    
                    auto rawangle = input.aimDirection;
                    auto dir = deg2rad(rawangle);
                    
                    int shooting = (input.inputs[Input::SHOOT] and not input.last_inputs[Input::SHOOT]);
                    if(shooting)
                    {
                        auto shotspeed = 800;
                        
                        new Bullet(Ent::New(), character->center_x(), character->center_y(), cos(dir) * shotspeed + hspeed, -sin(dir) * shotspeed, 1);
                        #ifdef CLIENT
                            fauxmix_emitter_fire(character->gun_emitter);
                        #endif
                    }
                    
                    float virtual_delta = 1.0/60;
                    int info = 0;
                    float temp = given_movement(hspeed, input, character, virtual_delta, &info);
                    temp -= hspeed;
                    temp /= virtual_delta;
                    temp *= delta;
                    temp += hspeed;
                    
                    int oldhsign = sign(hspeed);
                    
                    hspeed = temp;
                    
                    if(info & INFO_CHECKFAST)
                    {
                        if(abs(hspeed) > maxspeed)
                            hspeed = oldhsign * maxspeed;
                    }
                    if(info & INFO_CHECKSLOW)
                    {
                        if(abs(hspeed) < maxspeed)
                            hspeed = oldhsign * maxspeed;
                    }
                    if(info & INFO_CHECKREVERSE)
                    {
                        if(sign(hspeed) != oldhsign)
                            hspeed = 0;
                    }
                    
                    // Getting jump arcs to match 100% accurately is VERY HARD. So I handle the math manually,
                    // instead of relying on the short-circuited interp function.
                    float jumpspeed = -300;
                    
                    // Because http://i.imgur.com/MAhxJu6.png
                    // Assume zero delta as 1x gravity.
                    // Look at the right graph.
                    // When zero delta time, the arc will end at x=4, y=0.
                    // With dt = 1, arc ends at x=5
                    // dt = 2, arc ends at x=6,
                    // dt = 4. arc ends at x=8
                    // There will be some delta time where double gravity is appropriate.
                    // This is the delta time where, without the adjustment, the arc will end at 2x the distance.
                    // In this picture, that arc is the black arc.
                    // The left graph is a hypothetical "perfect" arc. Gravity here is actually cut in half relative to the above description.
                    // 
                    // The delta time where "the arc ends at 2x the distance" of the perfect arc, depends on the gravity and initial jump speed.
                    // Indeed, it's the delta time where gravity per frame is 2x the jumping velocity.
                    // Gravity per frame is basegravity*delta
                    // basegravity*delta == 2*jumpspeed -> gravity *= 2
                    // delta == 2*jumpspeed/basegravity -> gravity *= 2
                    //
                    // The following must also be assumed to be true.
                    // basegravity*delta == 0 -> gravity *= 1
                    //
                    // So it's obvious that [*= basegravity*delta/jumpspeed] can't be what we want; a delta of 0 would be *= 0.
                    // 
                    // It turns out, the value of delta is effectively a factor for how much extra gravity to add.
                    // To make the purple arc hit x=4 y=0, you need to add two to it by that point, or two across four times, or 0.5.
                    // To make the red arc hit x=4 7=0, you need to add four to it by that point, across two times, or 2.
                    // This is misleading: it appears to be growing quadratically, but it is not. If you count "four times" for that same period of time, it's 1.
                    // dt 1: 0.5 for intervals of 1
                    // dt 2: 2 for intervals of 2
                    // dt 4: 8 for intervals of 4
                    // 1 -> 0.5
                    // 2 -> 1
                    // 3 -> ???
                    // 4 -> 2
                    // 3 is 1.5, of course.
                    //
                    // The above values are relative to the grid lines in the picture.
                    // Let's look back on the "quadratically growing" thing.
                    // 1 -> 0.5
                    // 2 -> 2
                    // 3 -> ????
                    // 4 -> 8
                    //
                    // These are in terms of "per frame".
                    // Now, what does {delta = 2*jumpspeed/gravity} from earlier give us?
                    // With our numbers: ~0.833 seconds.
                    // At an infinitely high framerate, the jump would take 0.833 seconds to land.
                    // At 1.2fps, the jump would take twice that time to land.
                    // Let's just break that.
                    // 
                    // Let's review: *1 is when delta = 0, *2 is when delta is... a particular calculable value.
                    // And the "per frame" value grows proportionally to the frame time, just like gravity.
                    // We're effectively dealing with framegravity = deltagravity + f(delta, deltagravity), where "f" is basically just multiplication.
                    // "f" would return 0 for d=0, and 1 for d=[particular calculable value]
                    float gravity = basegravity*delta;
                    float gapdelta = fabs(2*jumpspeed/gravity);
                    gravity += delta/gapdelta*gravity;
                    
                    if(!place_meeting(character, 0, crop1(gravity)))
                    {
                        vspeed += gravity;
                        if(vspeed > max_gravity)
                            vspeed = max_gravity;
                    }
                    
                    int jumping = (input.inputs[Input::JUMP] & !input.last_inputs[Input::JUMP]);
                            
                    if(jumping)
                        vspeed = jumpspeed;
                    
                    int stepsize = 4;
                    
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
                        
                        auto running = (fabs(hspeed) > 1);
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
