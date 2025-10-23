#include "interpreter.h"
#include "xenonstd.h"
#include <iostream>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

Interpreter::Interpreter(lua_State* L){
    m_state = L;
    xenon::std::init(m_state);
}

int Interpreter::doFile(const char* file_path){
    bool result = luaL_dofile(m_state, file_path);

    if(result){
        const char* err = lua_tostring(m_state, -1);

        std::cerr 
        << std::endl 
        << "Xenon Exception" 
        << std::endl 
        << "  Uncaught Exception:" 
        << std::endl 
        << std::endl 
        << (err ? err : "unknown") 
        << std::endl;

        lua_pop(m_state, 1);
        return 1;
    }

    return 0;
}

int Interpreter::doArg(const Arg& arg) {

    if(arg.get_arg_type() == "-F") {
        return doFile(arg.get_arg().c_str());
    }

    return 0;
}