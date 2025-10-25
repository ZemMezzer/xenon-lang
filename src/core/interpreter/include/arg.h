#pragma once

#include <string>
#include <vector>
class Arg {
private:
    std::string m_arg_type;
    std::string m_arg;

public: 
    Arg(const char* arg_type, const char* arg);

    const std::string& get_arg() const;
    const std::string& get_arg_type() const;
};

class Args {
    private:
        std::vector<Arg> args;
    public:
        bool parse_args(int argc, char* argv[]);
        int size();
        const Arg& at(int index);
        bool has_arg(const std::string& arg_type);
};