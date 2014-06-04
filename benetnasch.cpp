#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional>
#include <cmath>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <nall/set.hpp>
#include "bmath.cpp"
#include "btime.cpp"
#include "sdlhelpers.cpp"

typedef long long entityid_t;

namespace Ent
{
    nall::set<entityid_t> Used; // IDs which are currently being used
    nall::set<entityid_t> Freed; // IDs which were used at some point, but no longer are
    entityid_t LowestUnused = 0; // The lowest ID which has never been used yet
    
    entityid_t New()
    {
        if (Freed.size() == 0) // No freed IDs, use lowest unused
        {
            auto id = LowestUnused;
            Used.insert(id); //logn
            
            #if DEBUG_IDALLOCATOR
                if(Used.find(LowestUnused)) // logn
                {
                    std::cout << "Error: Former LowestUnused set was invalid!";
                    return -1;
                }
            #endif
            
            // We need to figure out what the next lowest unused will be
            // There aren't any freed ones to use, so we have to do it hard
            if (LowestUnused >= *Used.end()) // lowest unused is outside of used range, free to increment LowestUnused
                LowestUnused++;
            else // This shouldn't ever happen; no freed IDs, lowest unused being inside Used's bounds -- implies earlier error setting LowestUnused
            {
                std::cout << "Error: Unknown cause with LowestUnused being invalid 1!";
                return -1;
            }
            return id;
        }
        else // use an ID from the freed list
        {
            auto id = *Freed.begin();
            Freed.remove(id); //logn
            Used.insert(id); //logn
            
            if(Freed.size() > 0) // We have an ID to use
            { // (Freed.size == 0 block won't be the next to run if we're here, so...)
                LowestUnused = *Freed.begin();
                return id;
            }
            else // We just exhausted the last previously freed ID
            {
                if (LowestUnused >= *Used.end()) // lowest unused is outside of used range, free to increment LowestUnused
                    LowestUnused++;
                else // This shouldn't ever happen; no freed IDs, lowest unused being inside Used's bounds -- implies earlier error setting LowestUnused
                {
                    std::cout << "Error: Unknown cause with LowestUnused being invalid 2!";
                    return -1;
                }
                return id;
            }
        }
    }
    // free a given ID -- returns false if id doesn't exist
    bool Free(entityid_t id) //perf: 3xlogn
    {
        if(Used.find(id))
            return false;
        Freed.insert(id);
        Used.remove(id);
        if(id < LowestUnused)
            LowestUnused = id;
        return true;
    }
    bool Exists(entityid_t id)
    {
        return Used.find(id);
    }
}

namespace Input
{
    enum {
        JUMP,
        LEFT,
        RIGHT,
        DOWN
    };
    bool inputs[256] = { }; 
    bool last_inputs[256] = { }; 
    signed char scrolls[4];
    const Uint8 * corestate;
    
    struct keybinding
    {
        SDL_Scancode key;
        unsigned char input_index;
    };
    std::vector<keybinding> keybindings;
    struct mousebinding
    {
        SDL_Scancode button;
        unsigned char input_index;
    };
    std::vector<mousebinding> mousebindings;
    
    void Init()
    {
        corestate = SDL_GetKeyboardState(NULL);
        keybindings.push_back({SDL_SCANCODE_E, JUMP});
        keybindings.push_back({SDL_SCANCODE_W, LEFT});
        keybindings.push_back({SDL_SCANCODE_D, DOWN});
        keybindings.push_back({SDL_SCANCODE_F, RIGHT});
    }
    
    void Update()
    {
        for (short i = 0; i < 256; i++)
        {
            last_inputs[i] = inputs[i];
            inputs[i] = 0;
        }
        for (auto bind : keybindings)
        {
            if(corestate[bind.key])
                inputs[bind.input_index] = true;
        }
        for (auto bind : mousebindings)
        {
            if(corestate[bind.button])
                inputs[bind.input_index] = true;
        }
    }
}

namespace Sys
{
    SDL_Window * MainWindow;
    SDL_Renderer * Renderer;
    SDL_Rect shape = {0, 0, 800, 600};
    
    bool quit = false;
    
    std::vector<bool(*)()> tems; // Sys::tems
    
    struct Component
    {
        Component(entityid_t myEntity);
        virtual ~Component();
        
        entityid_t entityID;
    };

    Component::Component(entityid_t myEntity) : entityID(myEntity)
    { }
    Component::~Component()
    { }
    
    // define collection and interfaces for components of some arbitrary type
    template<typename CType>
    struct Collection
    {
        std::vector<CType*> List;
        
        // Returns a pointer to the component which refers to a given entity
        //  or else 0 (if there is no such instance)
        CType * Refers(entityid_t myEntity)
        {
            unsigned i;
            for(i = 0;
                i < List.size()
                && List.at(i)->entityID != myEntity;
                ++i);
            
            return (i != List.size())?List[i]:0;
        }
        // Makes sure that there is some instance of this kind of component that
        // belongs to the given entity
        CType * EnforceDependency(entityid_t myEntity)
        {
            CType * relevant = Refers(myEntity);
            if(!relevant)
            {
                relevant = new CType(myEntity);
            }
            return relevant;
        }
        
        typename std::vector<CType*>::iterator begin()
        {
            return List.begin();
        }
        typename std::vector<CType*>::iterator end()
        {
            return List.end();
        }
    };
    
    // Position component
    struct Position : public Component
    {
        Position(entityid_t myEntity);
        double x, y;
    };
    // collection of positions
    Collection<Position> Positions;
    // Constructor will add this new instance to its list
    Position::Position(entityid_t myEntity) : Component(myEntity), x(0), y(0)
    {
        Positions.List.push_back(this);
    }
    
    // Hull component
    struct Hull : public Component
    {
        Hull(entityid_t myEntity);
        double h, w, xoffset, yoffset;
    };
    Collection<Hull> Hulls;
    Hull::Hull(entityid_t myEntity) : Component(myEntity), h(0), w(0), xoffset(0), yoffset(0)
    {
        Hulls.List.push_back(this);
    }
    
    // Sprite component
    struct TexturedDrawable : public Component
    {
        TexturedDrawable(entityid_t myEntity);
        Position * position;
        SDL_Texture * sprite;
        double xoffset, yoffset;
        bool init(const char * sarg);
    };
    Collection<TexturedDrawable> TexturedDrawables;
    TexturedDrawable::TexturedDrawable(entityid_t myEntity) : Component(myEntity), sprite(NULL), xoffset(0), yoffset(0)
    {
        position = Positions.EnforceDependency(myEntity);
        TexturedDrawables.List.push_back(this);
    }
    bool TexturedDrawable::init(const char * sarg)
    {
        sprite = loadTexture( sarg, Sys::Renderer );
        return sprite != nullptr;
    }
    
    // Character component
    struct Character : public Component
    {
        Character(entityid_t myEntity);
        Position * position;
        Hull * hull;
        TexturedDrawable * sprite;
        double hspeed, vspeed;
    };
    Collection<Character> Characters;
    Character::Character(entityid_t myEntity) : Component(myEntity), hspeed(0), vspeed(0)
    {
        hull = Hulls.EnforceDependency(myEntity);
        hull->h = 48;
        hull->w = 32;
        position = Positions.EnforceDependency(myEntity);
        sprite = TexturedDrawables.EnforceDependency(myEntity);
        sprite->init("sprites/mychar.png");
        Characters.List.push_back(this);
    }
    
    struct BoxDrawable : public Component
    {
        BoxDrawable(entityid_t myEntity);
        
        Position * position;
        Hull * hull;
        double xoffset, yoffset;
        
        SDL_Rect* getShape();
        
        private:
            SDL_Rect shape;
    };
    Collection<BoxDrawable> BoxDrawables;
    BoxDrawable::BoxDrawable(entityid_t myEntity) : Component(myEntity)
    {
        position = Positions.EnforceDependency(myEntity);
        hull = Hulls.EnforceDependency(myEntity);
        BoxDrawables.List.push_back(this);
    }
    SDL_Rect * BoxDrawable::getShape()
    {
        shape = {int(ceil(position->x+hull->xoffset)),
                 int(ceil(position->y+hull->yoffset)),
                 int(round(hull->w)),
                 int(round(hull->h))};
        return &shape;
    }
    
    bool FrameLimit()
    {
        // Frame limit
        double TimeWaitS;
        double TimeSpent;
        double prehalttime = Time::get_us(); // naively avoid error in non-critical debug code
        double halttime;
        
        if( Time::dostart )
        {
            SDL_Delay(1000.0/Time::Framerate);
            Time::dostart = false;
            Time::deviance = 0;
            halttime = Time::get_us();
        }
        else
        {
            // get time before halting
            // check time between end of last frame limit delay and current code
            TimeSpent = Time::get_us() - Time::simstart_us;
            // calculate desired amount to wait
            TimeWaitS = maximum(0.0, (1000.0 / Time::Framerate) - TimeSpent/1000.0 - Time::deviance);
            
            // store time for delay deviance check
            prehalttime = Time::get_us();
            SDL_Delay(round(TimeWaitS));
            
            // measure current time directly after frame limit delay
            halttime = Time::get_us();
            
            Time::error = (halttime - prehalttime)/1000.0 - round(TimeWaitS);
            // calculate amount of time that halt was inaccurate by and store it for the next iteration to compensate for
            Time::deviance = (halttime - prehalttime)/1000.0 - TimeWaitS;
            
            // incrememt tick count
            Time::ticks = fmod(Time::ticks + 1.0, Time::Framerate);
        }
        // push timings to buffer
        Time::last_us = Time::simstart_us;
        Time::delta_us = Time::simstart_us - Time::last_us;
        Time::delta = Time::delta_us * Time::scale;
        Time::simstart_us = halttime;
        Time::frames.push_back( Time::simstart_us );
        
        // Throw away old timings
        while ( Time::frames.size() > Time::Framesnum )
            Time::frames.erase( Time::frames.begin() );
        
        if(false) // debug
        {
            std::cout << std::fixed << std::setprecision(4)
                      <<"\rfps "    << Time::scale / ((Time::frames.back() - Time::frames.front())/(Time::frames.size()-1))
                      << " sim "    << std::setprecision(2)<< TimeSpent / 1000
                      //<< " bad "    << (Time::last - Time::simstart_us) * 1000
                      //<< " ask "    << std::setprecision(3) << TimeWaitS
                      << " halt "   << (halttime-prehalttime) / 1000
                      //<< " miss "   << std::setprecision(0) << Time::deviance * 1000
                      << " dev "    << Time::deviance << "\n";
        }
        return false;
    }
    bool SDLEvents()
    {
        SDL_PumpEvents();
        SDL_Event event;
        while ( SDL_PollEvent( &event ) )
        {
            switch ( event.type )
            {
            case SDL_QUIT:
                quit = true;
                break;
            }   
        }
        Input::Update();
        return false;
    }
    namespace Physicsers
    {
        bool MoveCharacters()
        {
            for(auto character : Sys::Characters)
            {
                
                /*
                 *  predef
                 */
                double &x = character->position->x;
                double &y = character->position->y;
                double &hspeed = character->hspeed;
                double &vspeed = character->vspeed;
                float delta = 1/Time::Framerate;
                
                /* set up muh functions */
                int stepsize = 4;
                
                auto place_meeting = [character](int x, int y)
                {
                    bool collided = false;
                    for(auto wallchunk : Sys::BoxDrawables)
                    {
                        auto place_meeting = [character, wallchunk](int x, int y)
                        {
                            return aabb_overlap(wallchunk->position->x, wallchunk->position->y,
                                                wallchunk->position->x+ wallchunk->hull->w, wallchunk->position->y+ wallchunk->hull->h,
                                                character->position->x + x, character->position->y + y,
                                                character->position->x + x + character->hull->w, character->position->y + y + character->hull->h);
                        };
                        
                        if(place_meeting(x, y))
                        {
                            collided = true;
                            break;
                        }
                    }
                    return collided;
                };
                auto move_contact = [&x, &y, place_meeting](double hvec, double vvec)
                {
                    // Move our position by a movement vector until we hit a filtered-in object or we finish the movement's magnitude.
                    
                    const int MAX_I = 64; // subpixel precision (MAX_I = 8 means 1/8th of a pixel)
                    int i = MAX_I;
                    
                    double maxDistance = vector_length(hvec, vvec); // maximum total travel
                    double sfac = maximum(absolute(hvec), absolute(vvec)); // Used to get pixel chunks from the movement vector
                   
                    double moveX = hvec/sfac*i/MAX_I,
                           moveY = vvec/sfac*i/MAX_I,
                           totalMoved = 0;
                   
                    while ( totalMoved < maxDistance and i > 0 )
                    {
                        moveX = hvec/sfac * i/MAX_I * minimum(1, maxDistance - totalMoved);
                        moveY = vvec/sfac * i/MAX_I * minimum(1, maxDistance - totalMoved);
                       
                        if (!place_meeting(x + moveX*i/MAX_I, y + moveY*i/MAX_I))
                        {
                            x += moveX * i/MAX_I;
                            y += moveY * i/MAX_I;
                            totalMoved += vector_length(moveX * i/MAX_I, moveY * i/MAX_I);
                           
                            if(i < MAX_I) // implies we have gotten contact before, then freed up
                            {
                                puts("Moved to contact...");
                                break;
                            }
                        }
                        else // got contact, eject from contact
                            i -= 1;
                    }
                 
                    return totalMoved;
                };
                
                /*
                 *  handle accelerations
                 */
                
                float taccel = 2000*delta;
                float fric_moving = pow(0.1, delta);
                float fric_counter = pow(0.01, delta);
                float fric_still = pow(0.02, delta);
                int crawlspeed = 75;
                int walkspeed = 250;
                int runspeed = 500;
                float struggle = 0.6;
                
                int direction = (Input::inputs[Input::RIGHT] - Input::inputs[Input::LEFT]);
                
                // If we're moving too slowly, our acceleration should be dampened
                if (direction == int(sign(hspeed)) or hspeed == 0)
                {
                    if (abs(hspeed) >= walkspeed)
                        ;
                    else if (abs(hspeed) <= crawlspeed)
                        taccel *= struggle;
                    else
                    {
                        float factor =
                          (float(abs(hspeed)) - crawlspeed)
                          / walkspeed;
                        taccel *= lerp(struggle, 1.0f, factor);
                    }
                }
                
                // calculate post-control speed
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
                    
                auto hsign = (0 < hspeed) - (hspeed < 0);
                
                if (abs(hspeed) > float(runspeed))
                    hspeed = hsign * runspeed;
                if (direction == 0)
                {
                    hspeed *= fric_still;
                    hspeed = absolute(hspeed) - 500*delta;
                    hspeed = hspeed > 0 ? hspeed : 0;
                    hspeed *= hsign;
                }
                
                if(!place_meeting(0, (vspeed+2000)*delta))
                {
                    vspeed += 2000*delta;
                    if(vspeed > 2000)
                        vspeed = 2000;
                    puts("GRAVITY");
                }
                
                /*
                 *  muh movement solving
                 */
                
                hspeed *= delta;
                vspeed *= delta;
                
                /* movement solver starts here */
                // we're in the wallmask
                if (place_meeting(0, 0))
                {
                    for (int i = 1; i < stepsize; i += 1)
                    {
                        if(!place_meeting(0, -i))
                        {
                            y -= i;
                            puts("eject up");
                            break;
                        }
                    }
                }
                // we collided with something
                else if (place_meeting(hspeed, vspeed))
                {
                    // check for up slopes and down sloped ceilings
                    auto oy = y;
                    for (int i = stepsize; i < hspeed; i += stepsize)
                    {
                        if(!place_meeting(hspeed, i))
                        {
                            y += i;
                            puts("upslope");
                            break;
                        }
                        if(!place_meeting(hspeed, -i))
                        {
                            y -= i;
                            puts("downceil");
                            break;
                        }
                    }
                    // no slope
                    if(y == oy)
                    {
                        move_contact(hspeed, vspeed);
                        // check for walls
                        if(place_meeting(sign(hspeed), 0))
                        {
                            puts("h");
                            hspeed = 0;
                        }
                        // assume floor otherwise
                        else if(place_meeting(0, sign(vspeed)))
                        {
                            puts("w");
                            vspeed = 0;
                        }
                        else
                        {
                            puts("Weird collision!");
                        }
                    }
                }
                // we did not collide with something
                else
                {
                    puts("nocol");
                    // we might want to "down" a slope
                    for (int i = 1; i < hspeed; i += stepsize)
                    {
                        if(!place_meeting(0, i) and place_meeting(0, i+1))
                        {
                            puts("downslope");
                            y += i;
                            break;
                        }
                    }
                }
                
                x += hspeed;
                y += vspeed;
                
                hspeed /= delta;
                vspeed /= delta;
            };
            return false;
        }
    }
    bool Physics()
    {
        Physicsers::MoveCharacters();
        return false;
    }
    namespace Renderers
    {
        bool TexturedDrawables()
        {
            for(auto drawable : Sys::TexturedDrawables)
            {
                renderTexture( drawable->sprite, Sys::Renderer, drawable->position->x, drawable->position->y );
            };
            return false;
        }
        bool BoxDrawables()
        {
            for(auto drawable : Sys::BoxDrawables)
            {
                SDL_SetRenderDrawColor( Sys::Renderer, 255, 255, 255, 255 );
                SDL_RenderFillRect( Sys::Renderer, drawable->getShape() );
            };
            return false;
        }
    }
    bool RenderThings()
    {
        // Clear screen
        // Cheap clear; use SDL_RenderClear() instead of SDL_RenderFillRect() if there are problems
        SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 255);
        SDL_RenderFillRect( Renderer, &shape );
        
        // Draw simple textured drawables
        Renderers::TexturedDrawables();
        Renderers::BoxDrawables();
        
        return false;
    }
    bool PresentScreen()
    {
        SDL_RenderPresent(Renderer);
        return false;
    }
}

bool sys_init()
{
    new Sys::Character(Ent::New());
    new Sys::BoxDrawable(Ent::New());
    auto d = Sys::BoxDrawables.List[0];
    
    d->position->x = 0;
    d->position->y = 120;
    d->hull->h = 50;
    d->hull->w = 400;
    
    Sys::tems.push_back(&Sys::FrameLimit);
    Sys::tems.push_back(&Sys::SDLEvents);
    Sys::tems.push_back(&Sys::Physics);
    Sys::tems.push_back(&Sys::RenderThings);
    Sys::tems.push_back(&Sys::PresentScreen);
    
    return 1;
}

bool main_init()
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
        std::cout << "Could not initialize SDL: " << SDL_GetError() << std::endl;

    int MAX_X = 800, MAX_Y = 600;

    Sys::MainWindow = SDL_CreateWindow("Benetnasch", 300, 300, MAX_X, MAX_Y, SDL_WINDOW_SHOWN);
    if (Sys::MainWindow == nullptr)
        std::cout << "Could not create an SDL window: " << SDL_GetError() << std::endl;
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_TARGETTEXTURE);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    srand(time(NULL));
    
    SDL_PumpEvents();
    Input::Init();
    
    Sys::tems.push_back(&sys_init);
    
    return 0;
}

int main(int argc, char *argv[])
{
    if (main_init())
        return 1;
    
    while ( !Sys::quit )
    {
        std::vector<bool(*)()> marked_for_removal;
        for(auto function : Sys::tems)
        {
            auto r = function();
            if(r)
                marked_for_removal.push_back(function);
        }
        for(auto function : marked_for_removal)
        {
            unsigned i;
            for (i = 0; Sys::tems[i] != function and i < Sys::tems.size(); ++i) ;
            if(i == Sys::tems.size())
                std::cout << "Bad system removal in mainloop!";
            else
                Sys::tems.erase(Sys::tems.begin()+i);
        }
    }
    return 0;
}
