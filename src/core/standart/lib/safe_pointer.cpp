#include "safe_pointer.h"
#include "lauxlib.h"
#include "lua.h"

static const char* lib_name = "safe_pointer";
static const char* value_out_of_range_for_type_exception_message = "Value out of range for target type";

void SafePointer::free_pointer() {
    if (value) {
        free(value);
        value = nullptr;
    }
}

static int l_ptr_index(lua_State* L) {
    auto ptr = (SafePointer*)luaL_checkudata(L, 1, lib_name);
    auto key = luaL_checkstring(L, 2);

    if (strcmp(key, "value") == 0) {

        if(ptr->type->is_boolean()){
            lua_pushboolean(L, *(bool*)ptr->value);
            return 1;
        }

        lua_pushnumber(L, ptr->type->to_number(ptr->value));
        return 1;
    }

    lua_pushnil(L);
    return 1;
}

static int l_ptr_newindex(lua_State* L) {
    auto ptr = (SafePointer*)luaL_checkudata(L, 1, lib_name);
    auto key = luaL_checkstring(L, 2);

    if (strcmp(key, "value") == 0) {

        lua_Number value;

        if(lua_isboolean(L, 3)) {
            value = lua_toboolean(L, 3);
        }
        else{
            value = luaL_checknumber(L, 3);
        }

        if(!ptr->type->is_overflow(value)){
            return luaL_error(L, value_out_of_range_for_type_exception_message);
        }

        ptr->type->assign(value, ptr->value);

        return 0;
    }

    return 0;
}

static int l_ptr_new_safe_ptr(lua_State* L) {
    auto ptr = (SafePointer*)lua_newuserdata(L, sizeof(SafePointer));
    auto type = lua_get_xenon_type(L);
    
    ptr->value = type->malloc();
    ptr->type = type;

    luaL_getmetatable(L, lib_name);
    lua_setmetatable(L, -2);

    return 1;
}

static int l_ptr_gc_collect(lua_State* L) {
    auto ptr = (SafePointer*)lua_touserdata(L, 1);

    if(ptr){
        ptr->free_pointer();
    }
    
    return 0;
}

extern "C" int luaopen_safeptr(lua_State* L) {
    luaL_newmetatable(L, lib_name);

    lua_pushcfunction(L, l_ptr_index);
    lua_setfield(L, -2, "__index");

    lua_pushcfunction(L, l_ptr_newindex);
    lua_setfield(L, -2, "__newindex");

    lua_pushcfunction(L, l_ptr_gc_collect);
    lua_setfield(L, -2, "__gc");

    lua_pushcfunction(L, l_ptr_new_safe_ptr);
    lua_setglobal(L, "new");

    return 0;
}