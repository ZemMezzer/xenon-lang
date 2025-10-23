#include <cstddef>
extern "C" {
#include "lua.h"
#include "lauxlib.h"
}
#include <cstdlib>
#include <cstring>
#include "pointer.h"

void Pointer::alloc(size_t e_size, size_t e_count){
    set((uint8_t*)malloc(size), e_size, e_count, true);
}

void Pointer::set(uint8_t* pointer_address, size_t e_size, size_t e_count, bool owned){

    element_size = e_size;
    elements_count = e_count;

    size = e_size * e_count;
    address = pointer_address;
    is_owned = owned;
}

void Pointer::free_pointer() {
    free(address);
    address = nullptr;
}

static const char* type_name = "Pointer";

static Pointer* check_ptr(lua_State* L, int idx) {
    return (Pointer*)luaL_checkudata(L, idx, type_name);
}

static int l_ptr_new(lua_State* L) {
    size_t e_size = luaL_checkinteger(L, 1);
    size_t e_count = luaL_checkinteger(L, 2);

    Pointer* p = (Pointer*)lua_newuserdata(L, sizeof(Pointer));
    p->alloc(e_size, e_count);
    luaL_getmetatable(L, type_name);
    lua_setmetatable(L, -2);
    return 1;
}

static int l_ptr_from(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    Pointer* np = (Pointer*)lua_newuserdata(L, sizeof(Pointer));
    np->set(p->address, p->element_size, p->elements_count, false);

    luaL_getmetatable(L, type_name);
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

static int l_ptr_getsize(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    lua_pushinteger(L, p->size);
    return 1;
}

static int l_ptr_get(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t index = luaL_checkinteger(L, 2);

    if (index >= p->elements_count) {
        return luaL_error(L, "BAD_ACCESS");
    }

    lua_Integer val = 0;
    std::memcpy(&val, p->address + index * p->element_size, p->element_size);

    lua_pushinteger(L, val);
    return 1;
}

static int l_ptr_set(lua_State* L) {
    Pointer* p = check_ptr(L, 1);
    size_t index = luaL_checkinteger(L, 2);
    int val = luaL_checkinteger(L, 3);

    if (index >= p->elements_count) {
        return luaL_error(L, "BAD_ACCESS");
    }

    std::memcpy(p->address + index * p->element_size, &val, p->element_size);
    return 0;
}

static const luaL_Reg ptr_methods[] = {
    {"free", l_ptr_free},
    {"get_byte", l_ptr_getbyte},
    {"set_byte", l_ptr_setbyte},
    {"get", l_ptr_get},
    {"set", l_ptr_set},
    {"get_size", l_ptr_getsize},
    {NULL, NULL}
};

static const luaL_Reg ptr_lib[] = {
    {"new", l_ptr_new},
    {"from", l_ptr_from},
    {NULL, NULL}
};

extern "C" int luaopen_ptr(lua_State* L) {
    luaL_newmetatable(L, type_name);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_setfuncs(L, ptr_methods, 0);

    luaL_newlib(L, ptr_lib);
    return 1;
}
