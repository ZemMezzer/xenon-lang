#pragma once

extern "C" {
#include "lua.h"
}

#include "arg.h"

class Interpreter {
public:
    int do_arg(const Arg& arg);
    int do_file(const char* file_path);
};