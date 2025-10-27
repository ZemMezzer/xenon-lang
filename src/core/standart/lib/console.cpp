#include "console.h"
#include <iostream>
#include "stack_helper.h"
extern "C" {
    #include "lauxlib.h"
}

static int l_console_readln(lua_State* L) {
    std::string input;
    std::getline(std::cin, input);

    lua_pushlstring(L, input.c_str(), input.size());
    return 1;
}

static int l_console_write(lua_State* L) {

    auto top_index = get_function_arg_top_index(L);
    auto args_n = lua_gettop(L);

    for (int i = top_index; i <= args_n; i++) {
        const int type = lua_type(L, i);

        switch (type) {
            case LUA_TSTRING:
                std::cout << lua_tostring(L, i) << std::flush;
            break;

            case LUA_TNUMBER:
                std::cout << lua_tonumber(L, i) << std::flush;
            break;

            case LUA_TBOOLEAN: {
                const char* cresult = lua_toboolean(L, i) == 0 ? "false" : "true";
                std::cout << cresult << std::flush;
            }  
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
    {"readln", l_console_readln},
    {NULL, NULL}
};

extern "C" int luaopen_console(lua_State* L) {
    luaL_newlib(L, console_lib);
    lua_setglobal(L, "console");

    return 0;
}