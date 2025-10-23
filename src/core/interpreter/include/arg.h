#pragma once

#include <string>
class Arg {
private:
    std::string m_arg_type;
    std::string m_arg;

public: 
    Arg(const char* arg_type, const char* arg);

    const std::string& get_arg() const;
    const std::string& get_arg_type() const;
};