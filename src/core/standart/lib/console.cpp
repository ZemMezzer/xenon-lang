#include <iostream>
#include "stack_helper.h"
extern "C" {
    #include "lauxlib.h"
    #include "lua.h"
}

static const char* type_name = "Console";

static int l_console_write(lua_State* L) {

    int top_index = get_function_arg_top_index(L);
    int args_n = lua_gettop(L);

    for (int i = top_index; i <= args_n; i++) {
        const int type = lua_type(L, i);

        switch (type) {
            case LUA_TSTRING:
                std::cout << lua_tostring(L, i) << std::flush;
            break;

            case LUA_TNUMBER:
                std::cout << lua_tonumber(L, i) << std::flush;
            break;

            case LUA_TBOOLEAN:
                std::cout << lua_toboolean(L, i) << std::flush;
            break;

            case LUA_TNIL:
                std::cout << "nil" << std::flush;
            break;

            case LUA_TTABLE:
                std::cout << "<table>" << std::flush;
            break;

            case LUA_TFUNCTION:
                std::cout << "<function>" << std::flush;
            break;

            case LUA_TUSERDATA:
                std::cout << "<userdata>" << std::flush;
            break;

            case LUA_TLIGHTUSERDATA:
                std::cout << "<lightuserdata>" << std::flush;
            break;

            case LUA_TTHREAD:
                std::cout << "<thread>" << std::flush;
            break;

            default:
                std::cout << "<unknown_type>" << std::flush;
            break;
        }
        
        if(i < args_n){
            std::cout << "\t" << std::flush;
        }
    }
    return 0;
}

static int l_console_write_line(lua_State* L) {
    l_console_write(L);
    std::cout << std::endl;

    return 0;
}

static const luaL_Reg console_lib[] = {
    {"write", l_console_write},
    {"writeln", l_console_write_line},
    {NULL, NULL}
};

extern "C" int luaopen_console(lua_State* L) {
    luaL_newmetatable(L, type_name);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_newlib(L, console_lib);
    lua_setglobal(L, "console");

    return 0;
}