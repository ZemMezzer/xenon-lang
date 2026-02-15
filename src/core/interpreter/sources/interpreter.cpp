#include "interpreter.h"
#include "xenonstd.h"
#include "runtime.h"
#include "filesystem.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

Interpreter::Interpreter(){
    m_state = luaL_newstate();
    xenon::std::init(m_state);
}

Interpreter::~Interpreter(){
    lua_close(m_state);
}

int Interpreter::do_file(const char* file_path){

    std::string full_path = lua_get_absolute_path(file_path);

    if(full_path.size() > 0){
        lua_set_home_path(lua_get_directory_path(full_path));
    }

    return xenon_do_file(m_state, file_path);
}

int Interpreter::do_arg(const Arg& arg) {

    if(arg.get_arg_type() == "-F") {
        return do_file(arg.get_arg().c_str());
    }

    return 0;
}