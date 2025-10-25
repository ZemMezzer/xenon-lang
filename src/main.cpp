#include <iostream>
#include <vector>
#include "arg.h"
#include "interpreter.h"

int main(int argc, char* argv[]) {

    std::vector<Arg> args;

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

    Interpreter interpreter;

    for (int i = 0; i < args.size(); i++) {
        const Arg& arg = args.at(i); 
        interpreter.do_arg(arg);
    }
    
    return 0;
}
