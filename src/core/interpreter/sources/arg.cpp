#include "arg.h"
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