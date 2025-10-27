#include "type.h"
#include "preprocessor.h"
#include "stack_helper.h"
#include <cstdlib>
#include <string>
#include <unordered_map>
#include <vector>
#include <functional>

extern "C" {
#include "lauxlib.h"
}

static const char* lib_name = "type";
static const char* invalid_type_exception_message = "Invalid type";

static char boolean_id;

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

    int top_inext = get_function_arg_top_index(L);

    if(!lua_isstring(L, top_inext)){
        luaL_error(L, invalid_type_exception_message);
        return nullptr;
    }

    std::string type_declaration = luaL_checkstring(L, top_inext);

    std::unordered_map<std::string, int>::iterator it = xenon_types.find(type_declaration);

    if(it == xenon_types.end()){
        luaL_error(L, invalid_type_exception_message);
        return nullptr;
    }

    return &types.at(it->second);
}

static std::string get_declaration_for(std::string keyword) {
    return "xenon:type:" + keyword;
}

bool Type::is_boolean() const {
    return type_id == boolean_id;
}

template<typename T>
static void* malloc_for(T value){
    void* ptr = malloc(sizeof(T));
    
    if (!ptr) {
        return nullptr;
    }

    std::memcpy(ptr, &value, sizeof(T));
    return ptr;
}

template<typename T>
static std::function<void*()> create_malloc_handle_for(T value){
   return [&]() {
        return malloc_for(value);
    };
}

template<typename T>
static std::function<bool(lua_Number)> create_overflow_handle_for(T){

    return [&](lua_Number number) {

        if (std::is_integral<T>::value) {
            return number >= static_cast<lua_Number>(std::numeric_limits<T>::min()) &&
                   number <= static_cast<lua_Number>(std::numeric_limits<T>::max());
        } else if (std::is_floating_point<T>::value) {
            return number >= -static_cast<lua_Number>(std::numeric_limits<T>::max()) &&
                   number <= static_cast<lua_Number>(std::numeric_limits<T>::max());
        }
        return false; 
    };
}

template<typename T>
static std::function<void(lua_Number, void*)> create_assign_handle_for(T) {
    return [](lua_Number value, void* ptr) {
        *(T*)ptr = static_cast<T>(value);
    };
}

template<typename T>
static std::function<lua_Number(void*)> create_to_number_handle_for(T) {
    return [](void* ptr) {
        return static_cast<lua_Number>(*(T*)ptr);
    };
}

template<typename T>
static void register_type(char id, const std::string& type_keyword, const size_t& size, T default_value){

    auto malloc_handle = create_malloc_handle_for(default_value);
    auto overflow_check_handle = create_overflow_handle_for(default_value);
    auto assign_handle = create_assign_handle_for(default_value);
    auto to_number_handle = create_to_number_handle_for(default_value);

    auto type_declaration = get_declaration_for(type_keyword);
    xenon_types[type_declaration] = types.size();
    types.emplace_back(
        Type(
            id,
            type_keyword.c_str(), 
            size, malloc_handle, 
            overflow_check_handle, 
            assign_handle,
            to_number_handle
        ));
    
    lua_register_keyword(type_keyword, "\""+type_declaration + "\"");
}

extern "C" int luaopen_types(lua_State* L) {

    register_type<char>(0,"byte", 1, 0);
    register_type<short>(1,"short", 2, 0);
    register_type<int>(2,"int", 4, 0);
    register_type<long>(3,"long", 8, 0);

    register_type<float>(4,"float", 4, 0);
    register_type<double>(5,"double", 8,  0);

    register_type<bool>(boolean_id = 6,"bool", 1, false);
    register_type<char>(7,"char", 1, 0);

    lua_register_global_functions(L);

    return 0;
}