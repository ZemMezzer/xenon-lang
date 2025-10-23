#include <cstddef>
#include <stdint.h>
extern "C" {
#include "lua.h"
}

class Pointer {
private:
    uint8_t* address;
    size_t size;
    Pointer* parent;

public:
    void alloc(size_t size);
    void set(uint8_t* pointer_address, size_t size, Pointer* parent);
    uint8_t* get_address();
    size_t get_size();
    bool is_owned();
    void free_pointer();
};

extern "C" int luaopen_ptr(lua_State* L);