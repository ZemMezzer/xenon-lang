#include "console.h"
#include "xstring.h"

#include <iostream>
#include <string>
#include "stack_helper.h"
extern "C" {
    #include "lauxlib.h"
}

static int xl_console_readln(lua_State* L) {
    std::string input;
    std::getline(std::cin, input);

    lua_pushlstring(L, input.c_str(), input.size());
    return 1;
}

static int xl_console_write(lua_State* L) {

    int top = lua_gettop(L);

    for (int i = 1; i <= top; i++) {

        luaL_tolstring(L, i, nullptr);
        std::cout << lua_tostring(L, -1);
        lua_pop(L, 1);

        if (i < top)
            std::cout << "\t" << std::flush;
    }

    return 0;
}

static int xl_console_write_line(lua_State* L) {
    xl_console_write(L);
    std::cout << std::endl;

    return 0;
}

static const luaL_Reg console_lib[] = {
    {"write", xl_console_write},
    {"writeln", xl_console_write_line},
    {"readln", xl_console_readln},
    {NULL, NULL}
};

extern "C" int xenon_openlib_console(lua_State* L) {
    luaL_newlib(L, console_lib);
    lua_setglobal(L, "console");

    return 0;
}