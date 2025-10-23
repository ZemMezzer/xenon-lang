#include <cstdint>
extern "C" {
#include "lua.h"
}

struct Pointer {
    uint8_t* address;
    bool is_owned;
    size_t size;

    void alloc(size_t size);
    void set(uint8_t* pointer_address, size_t size, bool owned);
    void free_pointer();
};

extern "C" int luaopen_ptr(lua_State* L);