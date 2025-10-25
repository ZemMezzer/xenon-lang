
#pragma once

#include <cstddef>
#include <string>
#include <unordered_map>

extern "C" {
#include "lua.h"
}

struct Type {
    const char* name;
    size_t size;

    Type(const char* tname, size_t tsize){
        name = tname;
        size = tsize;
    }
};

static std::unordered_map<std::string, int> xenon_types;

extern "C" int luaopen_types(lua_State* L);
const Type* lua_get_xenon_type(lua_State* L);