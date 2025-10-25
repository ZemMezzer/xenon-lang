#include <chrono>
#include <iostream>
#include <ostream>
#include <ratio>
#include "arg.h"
#include "interpreter.h"

typedef std::chrono::time_point<std::chrono::steady_clock, std::chrono::duration<long long, std::ratio<1LL, 1000000000LL> > > time_point;

static time_point start;

void begin_execution() {
    start = std::chrono::high_resolution_clock::now();
}

int complete_execution(int return_code, bool show_execution_time) {

    if(!show_execution_time){
        return return_code;
    }

    time_point end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;

    std::cout 
        <<std::endl
        << "Execution complete with code: "
        << return_code
        << std::endl
        << "Execution time: "
        << duration.count() 
        << " ms" 
        << std::endl
        << std::endl;

    return return_code;
}

int main(int argc, char* argv[]) {

    begin_execution();

    Args args;

    if(!args.parse_args(argc, argv)){
        complete_execution(1, false);
    }

    bool show_execution_time = false;
    Interpreter interpreter;

    for (int i = 0; i < args.size(); i++) {
        if(args.at(i).get_arg_type() == "--time"){
            show_execution_time = true;
            continue;
        }
    }

    for (int i = 0; i < args.size(); i++) {
        const Arg& arg = args.at(i); 
        int return_code = interpreter.do_arg(arg);

        if(return_code != 0){
            return complete_execution(1, show_execution_time);
        }
    }
    
    return complete_execution(0, show_execution_time);
}
