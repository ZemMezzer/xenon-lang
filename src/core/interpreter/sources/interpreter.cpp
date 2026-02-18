#include "interpreter.h"
#include "xenonstd.h"
#include "runtime.h"
#include "xdirectory.h"
#include "xerror.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

int Interpreter::do_file(const char* file_path){
    lua_State* l = luaL_newstate();
    xenon::std::init(l);

    std::string full_path = xenon_get_absolute_path(file_path);

    if(full_path.size() > 0){
        xenon_set_home_path(l, xenon_get_parent_path(full_path));
    }

	int status = xenon_do_file(l, file_path);

    if (status != LUA_OK) {
        xenon_throw(l);
    }

    return status;
}

int Interpreter::do_arg(const Arg& arg) {

    if(arg.get_arg_type() == "-F") {
        return do_file(arg.get_arg().c_str());
    }

    return 0;
}