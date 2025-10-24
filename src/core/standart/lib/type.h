
#pragma once

#include <cstddef>
#include <vector>

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

static std::vector<Type> xenon_types;

extern "C" int luaopen_types(lua_State* L);