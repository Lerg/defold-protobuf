#ifndef LUA_TABLE_DUMP_H
#define LUA_TABLE_DUMP_H

#include <dmsdk/sdk.h>

// Convert Lua value at a given index to a C-style string
void lua_value_to_string(lua_State *L, int index, char *output, size_t output_size);

// Shallow dump the contents of a Lua table into a C-style string
void lua_table_to_string(lua_State *L, int index, char *output, size_t output_size);

#endif // LUA_TABLE_DUMP_H
