#ifndef INCLUDE_BPLAYER
#define INCLUDE_BPLAYER

#include "../components.hpp"
#include "gamecomponents.hpp"
#include "../input.hpp"

namespace Sys
{
    struct Player : public Component
    {
        Player(entityid_t myEntity, const char * name);
        ~Player();
        void spawn(double x, double y);
        Character * character;
        
        std::string name;
        int pid;
        int pteam;
        int pclass;
        
        double spawntimer;
        int physics_frames_since_input_cycle;
        
        Input::PlayerInput input;
    };
    extern Collection<Player> Players;
}
#endif
