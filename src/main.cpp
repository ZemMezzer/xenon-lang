#include <iostream>
#include <vector>
#include "arg.h"
#include "interpreter.h"

extern "C" {
#include "lua.h"
#include "lauxlib.h"
}

int main(int argc, char* argv[]) {

    std::vector<Arg> args;

    lua_State* L = luaL_newstate();

    for (int i = 1; i < argc; i += 2) {

        if(i + 1 >= argc) {
            std::cout << "Invalid amout of arguments!";
            return 1;
        }

        const char* arg_type = argv[i];
        const char* arg = argv[i + 1];

        Arg ar(arg_type, arg);
        args.emplace_back(ar);
    }

    Interpreter interpreter(L);

    for (int i = 0; i < args.size(); i++) {
        const Arg& arg = args.at(i); 
        interpreter.doArg(arg);
    }

    lua_close(L);
    return 0;
}
