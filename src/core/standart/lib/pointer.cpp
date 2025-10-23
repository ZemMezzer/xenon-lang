extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <cstdlib>
#include <cstring>
#include <cstdint>
#include "pointer.h"

struct Pointer {
    uint8_t* address;
    bool is_owned;

    void set(uint8_t* pointer_address, bool owned){
        address = pointer_address;
        is_owned = owned;
    }

    void free_pointer(){
        free(address);
        address = nullptr;
    }
};

static Pointer* check_ptr(lua_State* L, int idx) {
    return (Pointer*)luaL_checkudata(L, idx, "Pointer");
}

static int l_ptr_new(lua_State* L) {
    size_t size = luaL_checkinteger(L, 1);
    Pointer* p = (Pointer*)lua_newuserdata(L, sizeof(Pointer));
    p->set((uint8_t*)malloc(size), true);
    luaL_getmetatable(L, "Pointer");
    lua_setmetatable(L, -2);
    return 1;
}

static int l_ptr_from(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    Pointer* np = (Pointer*)lua_newuserdata(L, sizeof(Pointer));
    np->set(p->address, false);

    luaL_getmetatable(L, "Pointer");
    lua_setmetatable(L, -2);
    return 1;
}

static int l_ptr_free(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    if (p->is_owned && p->address) {
        p->free_pointer();
    }
    return 0;
}

static int l_ptr_getbyte(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t offset = luaL_checkinteger(L, 2);
    lua_pushinteger(L, p->address[offset]);
    return 1;
}

static int l_ptr_setbyte(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t offset = luaL_checkinteger(L, 2);
    uint8_t val = luaL_checkinteger(L, 3);
    p->address[offset] = val;
    return 0;
}

static int l_ptr_get_int32(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t offset = luaL_checkinteger(L, 2);
    int32_t val;
    std::memcpy(&val, p->address + offset, sizeof(val));
    lua_pushinteger(L, val);
    return 1;
}

static int l_ptr_set_int32(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t offset = luaL_checkinteger(L, 2);
    int32_t val = luaL_checkinteger(L, 3);
    std::memcpy(p->address + offset, &val, sizeof(val));
    return 0;
}

static const luaL_Reg ptr_methods[] = {
    {"free", l_ptr_free},
    {"get_byte", l_ptr_getbyte},
    {"set_byte", l_ptr_setbyte},
    {"get_int32", l_ptr_get_int32},
    {"set_int32", l_ptr_set_int32},
    {NULL, NULL}
};

static const luaL_Reg ptr_lib[] = {
    {"new", l_ptr_new},
    {"from", l_ptr_from},
    {NULL, NULL}
};

extern "C" int luaopen_ptr(lua_State* L) {
    luaL_newmetatable(L, "Pointer");
    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");
    luaL_setfuncs(L, ptr_methods, 0);

    luaL_newlib(L, ptr_lib);
    return 1;
}
