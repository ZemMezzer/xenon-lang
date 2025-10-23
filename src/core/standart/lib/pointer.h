#include <cstddef>
#include <cstdint>
extern "C" {
#include "lua.h"
}

struct Pointer {
    uint8_t* address;
    bool is_owned;

    size_t element_size;
    size_t elements_count;

    size_t size;

    void alloc(size_t e_size, size_t e_count);
    void set(uint8_t* pointer_address, size_t e_size, size_t e_count, bool owned);
    void free_pointer();
};

extern "C" int luaopen_ptr(lua_State* L);