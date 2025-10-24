#include "type.h"
#include "preprocessor.h"
#include <string>
#include <vector>

extern "C" {
#include "lauxlib.h"
}

static const char* lib_name = "Type";

static const char* invalid_type_exception_message = "Invalid type";

int l_sizeof(lua_State* L) {

    if(!lua_isnumber(L, 1)){
        return luaL_error(L, invalid_type_exception_message);
    }

    int index = luaL_checkinteger(L, 1);

    if(index < 0 || index >= xenon_types.size()){
        return luaL_error(L, invalid_type_exception_message);
    }

    lua_pushinteger(L, xenon_types.at(index).size);
    return 1;
}

static void lua_register_global_functions(lua_State* L) {
    lua_pushcfunction(L, l_sizeof);
    lua_setglobal(L, "sizeof");
}

extern "C" int luaopen_types(lua_State* L) {

    xenon_types.emplace_back(Type("byte", 1));
    xenon_types.emplace_back(Type("short", 2));
    xenon_types.emplace_back(Type("int", 4));
    xenon_types.emplace_back(Type("long", 8));

    xenon_types.emplace_back(Type("float", 4));
    xenon_types.emplace_back(Type("double", 8));

    xenon_types.emplace_back(Type("bool", 1));
    xenon_types.emplace_back(Type("char", 1));

    for (int i = 0; i < xenon_types.size(); i++) {
        Type t = xenon_types.at(i);

        lua_pushinteger(L, i);
        lua_setglobal(L, t.name);
        lua_register_keyword(t.name, std::to_string(t.size));
    }

    lua_register_global_functions(L);

    return 0;
}