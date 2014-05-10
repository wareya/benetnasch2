#include <iostream>
#include <iomanip>
#include <algorithm>
#include <chrono>
#include <functional>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "bmath.cpp"
#include "btime.cpp"
#include "sdlhelpers.cpp"

typedef unsigned long entityid_t;

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
        void With(std::function<void(CType*)> lambda)
        {
            for(unsigned long i = 0; i != List.size(); ++i)
            {
                lambda(List[i]);
            }
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
        double h, w;
    };
    Collection<Hull> Hulls;
    Hull::Hull(entityid_t myEntity) : Component(myEntity), h(0), w(0)
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
        position = Positions.EnforceDependency(myEntity);
        sprite = TexturedDrawables.EnforceDependency(myEntity);
        sprite->init("sprites/mychar.png");
        Characters.List.push_back(this);
    }
    
    bool FrameLimit()
    {
        // Frame limit
        double TimeWaitS;
        double TimeSpent;
        double prehalttime = Time::get_us(); // naively prevent error in non-critical debug code
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
        while ( Time::frames.size() > 125 )
            Time::frames.erase( Time::frames.begin() );
        
        if(true) // debug
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
        return false;
    }
    namespace Physicsers
    {
        bool MoveCharacters()
        {
            Sys::Characters.With([](Character * component)
            {
                double &x = component->position->x;
                double &y = component->position->y;
                if (y == 0 and x < 800-32)
                    x += 1;
                else if (x == 800-32 and y < 600-48)
                    y += 1;
                else if (x > 0)
                    x -= 1;
                else
                    y -= 1;
            });
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
            Sys::TexturedDrawables.With([](TexturedDrawable * component)
            {
                renderTexture( component->sprite, Sys::Renderer, component->position->x, component->position->y );
            });
            return false;
        }
    }
    bool RenderThings()
    {
        // Clear screen
        // Cheap clear; use SDL_RenderClear() instead of SDL_RenderFillRect() if there are problems
        SDL_SetRenderDrawColor( Renderer, 0, 0, 0, 255);
        SDL_RenderFillRect( Renderer, &shape );
        
        Renderers::TexturedDrawables();
        
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
    new Sys::Character(0);
    
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
    Sys::Renderer = SDL_CreateRenderer(Sys::MainWindow, -1, SDL_RENDERER_ACCELERATED || SDL_RENDERER_PRESENTVSYNC);
    if (Sys::Renderer == nullptr)
        std::cout << "Could not create an SDL renderer: " << SDL_GetError() << std::endl;
    
    srand(time(NULL));
    
    Sys::tems.push_back(&sys_init);
    
    SDL_PumpEvents();
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
