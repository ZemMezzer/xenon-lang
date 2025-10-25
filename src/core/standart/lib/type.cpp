#include "type.h"
#include "preprocessor.h"
#include <string>
#include <unordered_map>
#include <vector>

extern "C" {
#include "lauxlib.h"
}

static const char* lib_name = "Type";
static const char* invalid_type_exception_message = "Invalid type";

static std::vector<Type> types;

int l_sizeof(lua_State* L) {

    const Type* type = lua_get_xenon_type(L);

    if(type == nullptr){
        return 0;
    }

    lua_pushinteger(L, type->size);
    return 1;
}

static void lua_register_global_functions(lua_State* L) {
    lua_pushcfunction(L, l_sizeof);
    lua_setglobal(L, "sizeof");
}

const Type* lua_get_xenon_type(lua_State* L) {

    if(!lua_isstring(L, 1)){
        luaL_error(L, invalid_type_exception_message);
        return nullptr;
    }

    std::string type_declaration = luaL_checkstring(L, 1);

    std::unordered_map<std::string, int>::iterator it = xenon_types.find(type_declaration);

    if(it == xenon_types.end()){
        luaL_error(L, invalid_type_exception_message);
        return nullptr;
    }

    return &types.at(it->second);
}

static void register_type(const std::string& type_keyword, const size_t& size){
    std::string type_declaration = "xenon:type:" + type_keyword;
    xenon_types[type_declaration] = types.size();
    types.emplace_back(Type(type_keyword.c_str(), size));
    lua_register_keyword(type_keyword, "\""+type_declaration + "\"");
}

extern "C" int luaopen_types(lua_State* L) {

    register_type("byte", 1);
    register_type("short", 2);
    register_type("int", 4);
    register_type("long", 8);

    register_type("float", 4);
    register_type("double", 8);

    register_type("bool", 1);
    register_type("char", 1);

    lua_register_global_functions(L);

    return 0;
}