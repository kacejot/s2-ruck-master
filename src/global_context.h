#pragma once
#include <windows.h>
#include "local_types.h"
#include "config.h"
#include "print.h"

template <typename T>
class singleton
{
    singleton(const singleton&) = delete;
    singleton& operator=(const singleton&) = delete;

public:
    static T& instance()
    {
        static T s_instance;
        return s_instance;
    }

protected:
    singleton() = default;
    ~singleton() = default;
};

struct global_context : public singleton<global_context>
{
	ruck_master_config config;
    known_functions functions;
    uintptr_t game_base;

    global_context()
    {
        game_base = (uintptr_t)GetModuleHandleA(nullptr);
        functions =
        {
            FOREIGN_FUNCTION(get_global_state_t,         game_base + known_function_offsets[GET_GLOBAL_STATE]),
            FOREIGN_FUNCTION(get_item_by_descriptor_t,   game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]),
            FOREIGN_FUNCTION(get_item_metadata_t,        game_base + known_function_offsets[GET_ITEM_METADATA]),
            FOREIGN_FUNCTION(get_weapon_from_item_t,     game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]),
            FOREIGN_FUNCTION(get_secondary_sort_key_t,   game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]),
            FOREIGN_FUNCTION(get_item_name_t,            game_base + known_function_offsets[GET_ITEM_NAME]),
            FOREIGN_FUNCTION(compare_names_t,            game_base + known_function_offsets[COMPARE_ITEM_NAMES]),
            FOREIGN_FUNCTION(free_s2string_t,            game_base + known_function_offsets[FREE_ITEM_NAME]),
        };
    }
};

inline global_context& get_ctx()
{
	return global_context::instance();
}

inline known_functions& get_functions()
{
    return get_ctx().functions;
}
