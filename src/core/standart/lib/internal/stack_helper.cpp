#include "stack_helper.h"

std::string stack_value_to_string(lua_State* L, int idx) {
    size_t len;
    const char* str = luaL_tolstring(L, idx, &len);

    std::string result(str, len);

    lua_pop(L, 1);

    return result;
}