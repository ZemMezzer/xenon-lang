#include "arg.h"
#include <iostream>
#include <string>

Arg::Arg(const char* arg_type, const char* arg){
    m_arg_type = arg_type;
    m_arg = arg;
}

const std::string& Arg::get_arg() const {
    return m_arg;
}

const std::string& Arg::get_arg_type() const {
    return m_arg_type;
}

static bool is_non_typed_arg(const std::string& arg){
    return arg.size() >= 2 && arg[0] == '-' && arg[1] == '-';
}

bool Args::parse_args(int argc, char* argv[]) {
    for (int i = 1; i < argc; i++) {
        
        if(is_non_typed_arg(argv[i])){
            Arg narg(argv[i], "");
            args.emplace_back(narg);
            continue;
        }

        if(i + 1 >= argc) {
            std::cout << "Invalid amout of arguments!";
            return false;
        }

        const char* arg_type = argv[i];
        const char* arg = argv[i + 1];

        Arg ar(arg_type, arg);
        args.emplace_back(ar);
        i++;
    }

    return true;
}

const Arg& Args::at(int index) {
    return args.at(index);
}

int Args::size() {
    return args.size();
}