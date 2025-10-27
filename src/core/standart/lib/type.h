
#pragma once

#include <cstddef>
#include <functional>
#include <string>
#include <unordered_map>

extern "C" {
#include "lua.h"
}

struct Type {
    std::function<void*()> malloc;
    std::function<bool(lua_Number)> is_overflow;
    std::function<void(lua_Number, void*)> assign;
    std::function<lua_Number(void*)> to_number;

    char type_id;
    const char* name;
    size_t size;

    Type(
        char id,
        const char* tname, 
        size_t tsize, 
        std::function<void*()> malloc_handle, 
        std::function<bool(lua_Number)> is_overflow_handle, 
        std::function<void(lua_Number, void*)> assingn_handle,
        std::function<lua_Number(void*)> to_number_handle)
    {
        type_id = id;
        name = tname;
        size = tsize;

        malloc = malloc_handle;
        is_overflow = is_overflow_handle;
        assign = assingn_handle;
        to_number = to_number_handle;
    }

    bool is_boolean() const;
};

static std::unordered_map<std::string, int> xenon_types;

extern "C" int luaopen_types(lua_State* L);
const Type* lua_get_xenon_type(lua_State* L);