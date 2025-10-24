#include <string>
#include <unordered_map>
#include "preprocessor.h"

static std::unordered_map<std::string, std::string> keywords_map;

void lua_register_keyword(const std::string& keyword, const std::string& result) {
    keywords_map[keyword] = result;
}

std::string lua_preprocess_code(const std::string& code) {
    std::string result;
    size_t i = 0;
    bool in_string = false;
    char string_quote = 0;
    bool in_line_comment = false;
    bool in_block_comment = false;

    while (i < code.size()) {
        char c = code[i];

        if (in_string) {
            result += c;
            if (c == '\\') {
                i++;
                if (i < code.size()) result += code[i];
            } else if (c == string_quote) {
                in_string = false; 
            }
            i++;
        }

        else if (in_line_comment) {
            result += c;
            if (c == '\n') in_line_comment = false;
            i++;
        }
        else if (in_block_comment) {
            result += c;
            if (c == ']' && i+1 < code.size() && code[i+1] == ']') {
                result += ']';
                i += 2;
                in_block_comment = false;
            } else {
                i++;
            }
        }

        else if (c == '"' || c == '\'') {
            in_string = true;
            string_quote = c;
            result += c;
            i++;
        }
        else if (c == '-' && i+1 < code.size() && code[i+1] == '-') {
            result += "--";
            i += 2;
            if (i < code.size() && code[i] == '[' && i+1 < code.size() && code[i+1] == '[') {
                result += "[[";
                i += 2;
                in_block_comment = true;
            } else {
                in_line_comment = true;
            }
        }

        else if (std::isalpha(c) || c == '_') {
            size_t start = i;
            while (i < code.size() && (std::isalnum(code[i]) || code[i] == '_')) i++;
            std::string token = code.substr(start, i - start);

            std::unordered_map<std::string, std::string>::const_iterator it = keywords_map.find(token);

            if (it != keywords_map.end()) {
                result += it->second;
            } else {
                result += token;
            }
        }

        else {
            result += c;
            i++;
        }
    }

    return result;
}