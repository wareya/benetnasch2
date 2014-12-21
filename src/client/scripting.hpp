#ifndef INCLUDE_CLIENTSCRIPTING
#define INCLUDE_CLIENTSCRIPTING

#include <lua/lua.hpp>

namespace Lua
{
    extern lua_State * LuaHUD;
    
    bool do_hud();

    int tolua_print (lua_State * L);

    void scripting_init();
}

#endif
