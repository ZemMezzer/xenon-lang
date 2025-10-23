#include "stack_helper.h"

int get_function_arg_top_index(lua_State* L){
    if(lua_istable(L, 1)) {
        return 2;
    } 
    return 1;
}