#include "subroutines.hpp"
#include "../input.hpp"
#include "../components/player.hpp"
#include <vector>

#ifdef CLIENT
#include "../samples.hpp"
#endif

static const float basegravity = 720;

struct movedata
{
    double hspeed, vspeed;
    void operator+=(movedata k)
    {
        hspeed += k.hspeed;
        vspeed += k.vspeed;
    }
    void operator-=(movedata k)
    {
        hspeed -= k.hspeed;
        vspeed -= k.vspeed;
    }
    void  operator*=(float f)
    {
        hspeed *= f;
        vspeed *= f;
    }
    void  operator/=(float f)
    {
        hspeed /= f;
        vspeed /= f;
    }
};


namespace Sys
{
    namespace Physicsers
    {
        // Benetnasch predicts movement and rescales it based on the actual time taken
        // in order to compromise on the advantages of both delta time and interpolated timestep.
        movedata given_movement(movedata mine, Input::PlayerInput input, Sys::Character * character, float delta)
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
            float maxspeed = 300;
            float gravity = basegravity*delta;
            float max_gravity = 2000;
            
            double hspeed = mine.hspeed;
            double vspeed = mine.vspeed;
            
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
                    if(speed > maxspeed)
                        speed = maxspeed;
                    hspeed = direction * speed;
                }
                else
                {   // if we're agreeing with our movement, but STARTED going too fast,
                    // we need to use a completely different model in order to
                    // agree with the non-direction_agreement case.
                    // However, it still needs to be here (rather than non-direction_agreement)
                    // in order to gracefully go to maxspeed.
                    speed -= deaccel;
                    if(speed < maxspeed)
                        speed = maxspeed;
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
                
                if(fabs(hspeed) < deaccel and direction == 0)
                    hspeed = 0;
                else
                    hspeed -= sign(hspeed)*apply;
            }
            
            if(!place_meeting(character, 0, crop1(gravity)))
            {
                vspeed += gravity;
                if(vspeed > max_gravity)
                    vspeed = max_gravity;
            }
            
            return movedata({hspeed, vspeed});
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
                    
                    movedata temp = {hspeed, vspeed};
                    float virtual_delta = 1.0/60;
                    auto tossed = given_movement(temp, input, character, virtual_delta);
                    tossed -= temp;
                    tossed /= virtual_delta;
                    tossed *= delta;
                    tossed += temp;
                    
                    hspeed = tossed.hspeed;
                    vspeed = tossed.vspeed;
                    
                    // Because http://i.imgur.com/MAhxJu6.png
                    float applied_gravity = basegravity*Time::Frametime/1000;
                    // Behavior: Supposing a given jump consists of only the frames "on the ground", "at the apex", and "on the ground"
                    //              it will have double the length of arc that a "perfectly accurate" jump has.
                    // Such a jump has a per-frame gravity equal to double the jumpspeed.
                    // We're going to correct all jumps to look like this "perfectly accurate" jump.
                    // To do this, we pre-apply half of a real frame of gravity.
                    // We do it based on desired frametime instead of actual delta, because.... Sorry. It's literally impossible to do any better.
                    // TODO: Correct for this in GRAVITY instead! SERIOUSLY! (How? -> Get help!) That's where this belongs!
                    //   Scratch notes: If done in gravity, gravity would either decrease towards high framerates, or increase towards low framerates. Mull.
                    float jumpspeed = (-300) + (applied_gravity/2);
                    
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
