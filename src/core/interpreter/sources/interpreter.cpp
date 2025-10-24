#include "interpreter.h"
#include "xenonstd.h"
#include "preprocessor.h"
#include <fstream>
#include <iostream>
#include <sstream>

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

Interpreter::Interpreter(lua_State* L){
    m_state = L;
    xenon::std::init(m_state);
}

int Interpreter::doFile(const char* file_path){

    std::ifstream file(file_path);
    if (!file.is_open()) {
        std::cerr << "Unable to open file " << file_path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string processedCode = lua_preprocess_code(buffer.str());

    if (luaL_loadbuffer(m_state, processedCode.c_str(), processedCode.size(), file_path) || lua_pcall(m_state, 0, LUA_MULTRET, 0)) {
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