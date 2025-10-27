#include "pointer.h"
#include "lua.h"
#include "stack_helper.h"
#include "type.h"
#include <stdlib.h>
#include <cstring>

extern "C" {
#include "lauxlib.h"
}

void Pointer::alloc(size_t size){
    set((uint8_t*)malloc(size), size, nullptr);
}

void Pointer::set(uint8_t* address, size_t size, Pointer* parent){    
    this->size = size;
    this->parent = parent;

    if (!parent) {
        this->address = address;
    } else {
        this->address = nullptr;
    }
}

void Pointer::free_pointer() {
    free(address);
    address = nullptr;
    size = 0;
}

uint8_t* Pointer::get_address(){
    if(parent){
        return parent->get_address();
    }
    else {
        return address;
    }
}

size_t Pointer::get_size() {
    return size;
}

bool Pointer::is_owned() {
    return parent == nullptr;
}

static const char* lib_name = "pointer";

static const char* nil_ptr_exception_message = "Attempt to access nil pointer";
static const char* non_owned_ptr_free_exception_message = "Attempt to free non-owned pointer";
static const char* mem_access_out_of_bounds_exception_message = "Memory access out of bounds";

static Pointer* check_ptr(lua_State* L, int idx) {
    return (Pointer*)luaL_checkudata(L, idx, lib_name);
}

static int l_ptr_new(lua_State* L) {

    int top_index = get_function_arg_top_index(L);

    size_t args_size = lua_gettop(L) - top_index;
    size_t size = luaL_checkinteger(L, top_index);

    auto p = (Pointer*)lua_newuserdata(L, sizeof(Pointer));

    p->alloc(size);
    luaL_getmetatable(L, lib_name);

    lua_setmetatable(L, -2);

    return 1;
}

static int l_ptr_from(lua_State* L) {

    auto top_index = get_function_arg_top_index(L);

    auto p = check_ptr(L, top_index);
    auto np = (Pointer*)lua_newuserdata(L, sizeof(Pointer));
    np->set(nullptr, p->get_size(), p);

    luaL_getmetatable(L, lib_name);
    lua_setmetatable(L, -2);
    return 1;
}

static int l_ptr_free(lua_State* L) {
    auto p = check_ptr(L, 1);
    if (p->is_owned() && p->get_address()) {
        p->free_pointer();
        return 0;
    }

    if(!p->get_address()){
        return 0;
    }
    
    if(!p->is_owned()) {
        return luaL_error(L, non_owned_ptr_free_exception_message);
    }

    return 0;
}

static int l_ptr_getbyte(lua_State* L) {
    auto p = check_ptr(L, 1);

    if(!p->get_address()){
        luaL_error(L, nil_ptr_exception_message);
        return 0;
    }

    size_t offset = luaL_checkinteger(L, 2);

    if (offset >= p->get_size()) {
        return luaL_error(L, mem_access_out_of_bounds_exception_message);
    }

    lua_pushinteger(L, p->get_address()[offset]);
    return 1;
}

static int l_ptr_setbyte(lua_State* L) {
    auto p = check_ptr(L, 1);

    if(!p->get_address()){
        return luaL_error(L, nil_ptr_exception_message);;
    }

    size_t offset = luaL_checkinteger(L, 2);

    if (offset >= p->get_size()) {
        return luaL_error(L, mem_access_out_of_bounds_exception_message);
    }

    uint8_t val = luaL_checkinteger(L, 3);
    p->get_address()[offset] = val;
    return 0;
}

static int l_ptr_getsize(lua_State* L) {
    auto p = check_ptr(L, 1);
    lua_pushinteger(L, p->get_size());
    return 1;
}

static const luaL_Reg ptr_methods[] = {
    {"free", l_ptr_free},
    {"get_byte", l_ptr_getbyte},
    {"set_byte", l_ptr_setbyte},
    {"get_size", l_ptr_getsize},
    {NULL, NULL}
};

static const luaL_Reg ptr_lib[] = {
    {"new", l_ptr_new},
    {"from", l_ptr_from},
    {NULL, NULL}
};

extern "C" int luaopen_ptr(lua_State* L) {

    luaL_newmetatable(L, lib_name);

    lua_pushvalue(L, -1);
    lua_setfield(L, -2, "__index");

    luaL_setfuncs(L, ptr_methods, 0);

    luaL_newlib(L, ptr_lib);
    lua_setglobal(L, "ptr");

    return 0;
}
