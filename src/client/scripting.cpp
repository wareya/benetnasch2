#include "scripting.hpp"

#include <lua/lua.hpp>
#include <iostream>

namespace Lua
{
    lua_State * LuaHUD;
    
    bool do_hud()
    {
        lua_pushvalue(LuaHUD, -1);
        lua_pcall(LuaHUD, 0, 0, 0);
        return 0;
    }

    int tolua_print (lua_State * L)
    {
        std::cout << lua_tostring(L, 1) << "\n";
        return 0;
    }

    void scripting_init()
    {
        LuaHUD = luaL_newstate();
        if (LuaHUD == nullptr)
            std::cout << "Could not load LUA. Sorry.\n";
        
        luaL_Reg funcs[] = 
        {
            {"print", &tolua_print},
            {NULL,NULL}
        };
        
        luaL_newlib(LuaHUD, funcs);
        lua_setglobal(LuaHUD, "bhud");
        
        int afwe = luaL_loadfile(LuaHUD, "scripts/hud.lua");
        if (afwe != 0)
            std::cout << "Could not load HUD script: " << afwe << std::endl;
    }
}
