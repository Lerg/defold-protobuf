#include "lua_table_dump.h"

static const uint32_t BUFFER_SIZE = 1024;

// Function to convert relative index to absolute index in Lua 5.1
int lua_absindex(lua_State *L, int index) {
    if (index > 0 || index <= LUA_REGISTRYINDEX) {
        return index;
    }
    return lua_gettop(L) + index + 1;
}

// Count the number of array and dictionary elements in a Lua table
static void count_table_elements(lua_State *L, int index, int *array_size, int *dict_size) {
    index = lua_absindex(L, index);
    *array_size = 0;
    *dict_size = 0;

    lua_pushnil(L); // Initial key
    while (lua_next(L, index) != 0) {
        if (lua_type(L, -2) == LUA_TNUMBER && lua_tonumber(L, -2) == (lua_Number)lua_tointeger(L, -2)) {
            // Integer key, part of array
            ++(*array_size);
        } else {
            // Non-integer key, part of dictionary
            ++(*dict_size);
        }
        lua_pop(L, 1); // Pop value, keep key
    }
}

void lua_table_to_string(lua_State *L, int index, char *output, size_t output_size) {
    index = lua_absindex(L, index);
    strncat(output, "{", output_size - strlen(output) - 1);
    if (lua_istable(L, index)) {
        lua_pushnil(L);  // First key
        while (lua_next(L, index) != 0) {
            // Get key and value
            char key_str[BUFFER_SIZE] = {0};
            char value_str[BUFFER_SIZE] = {0};

            lua_value_to_string(L, -2, key_str, sizeof(key_str));

            if (lua_istable(L, -1)) {
                // Handle inner tables with table(N,K) format
                int array_size = 0, dict_size = 0;
                count_table_elements(L, lua_gettop(L), &array_size, &dict_size);
                snprintf(value_str, BUFFER_SIZE, "table(%d,%d)", array_size, dict_size);
            } else {
                lua_value_to_string(L, -1, value_str, sizeof(value_str));
            }

            // Add key-value pair to output
            strncat(output, key_str, output_size - strlen(output) - 1);
            strncat(output, ":", output_size - strlen(output) - 1);
            strncat(output, value_str, output_size - strlen(output) - 1);
            strncat(output, ",", output_size - strlen(output) - 1);

            lua_pop(L, 1);  // Remove value, keep key for the next iteration
        }

        // Remove the last comma
        if (output[strlen(output) - 1] == ',') {
            output[strlen(output) - 1] = '\0';
        }
    } else {
        lua_value_to_string(L, index, output, output_size);
    }
    strncat(output, "}", output_size - strlen(output) - 1);
}

void lua_value_to_string(lua_State *L, int index, char *output, size_t output_size) {
    int type = lua_type(L, index);
    switch (type) {
        case LUA_TSTRING:
            strncat(output, "\"", output_size - strlen(output) - 1);
            strncat(output, lua_tostring(L, index), output_size - strlen(output) - 1);
            strncat(output, "\"", output_size - strlen(output) - 1);
            break;
        case LUA_TBOOLEAN:
            strncat(output, lua_toboolean(L, index) ? "true" : "false", output_size - strlen(output) - 1);
            break;
        case LUA_TNUMBER:
            char num_str[BUFFER_SIZE];
            snprintf(num_str, BUFFER_SIZE, "%g", lua_tonumber(L, index));
            strncat(output, num_str, output_size - strlen(output) - 1);
            break;
        case LUA_TNIL:
            strncat(output, "nil", output_size - strlen(output) - 1);
            break;
        case LUA_TTABLE:
            strncat(output, "table", output_size - strlen(output) - 1);
            break;
        case LUA_TUSERDATA:
        case LUA_TLIGHTUSERDATA:
            strncat(output, "userdata", output_size - strlen(output) - 1);
            break;
        case LUA_TFUNCTION:
            strncat(output, "function", output_size - strlen(output) - 1);
            break;
        default:
            strncat(output, "unknown", output_size - strlen(output) - 1);
            break;
    }
}
