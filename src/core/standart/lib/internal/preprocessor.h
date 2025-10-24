#include <string>

void lua_register_keyword(const std::string& keyword, const std::string& result);
std::string lua_preprocess_code(const std::string& code);