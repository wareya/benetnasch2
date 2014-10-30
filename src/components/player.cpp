#include "gamecomponents.hpp"
#include "player.hpp"

namespace Sys
{
    Player::Player(entityid_t myEntity, const char * name) : Component(myEntity), name(name)
    {
        character = nullptr;
        pid = 0; // TODO: FIX IMMEDIATELY
        pteam = 0; // TODO: enum
        pclass = 0; // TODO: enum
        spawntimer = -1;
        physics_frames_since_input_cycle = 0;
        Players.add(this);
        puts("MAKING A PLAYER");
    }
    Player::~Player()
    {
        delete character;
        Players.remove(this);
    }
    void Player::spawn(double x, double y)
    {
        character = new Character(entityID, x, y);
    }
    Collection<Player> Players;
}
