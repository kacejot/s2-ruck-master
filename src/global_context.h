#pragma once
#include <windows.h>
#include "local_types.h"
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
    uintptr_t game_base;
    known_functions functions;
	known_functions wrappers;

    global_context()
    {
        game_base = (uintptr_t)GetModuleHandleA(nullptr);
        functions =
        {
           (get_global_state_t)(game_base + known_function_offsets[GET_GLOBAL_STATE]),
           (get_item_by_descriptor_t)(game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]),
           (get_item_metadata_t)(game_base + known_function_offsets[GET_ITEM_METADATA]),
           (get_weapon_from_item_t)(game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]),
           (get_secondary_sort_key_t)(game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]),
           (get_item_name_t)(game_base + known_function_offsets[GET_ITEM_NAME]),
           (compare_names_t)(game_base + known_function_offsets[COMPARE_ITEM_NAMES]),
           (free_s2string_t)(game_base + known_function_offsets[FREE_ITEM_NAME]),
        };

        wrappers =
        {
            WRAP_FN(functions.get_global_object_pool),
            WRAP_FN(functions.get_item_by_descriptor),
            WRAP_FN(functions.get_item_metadata),
            WRAP_FN(functions.get_weapon_from_item),
            WRAP_FN(functions.get_secondary_sort_key),
            WRAP_FN(functions.get_item_name),
            WRAP_FN(functions.compare_names),
            WRAP_FN(functions.free_s2string),
        };
    }
};

inline global_context& get_ctx()
{
	return global_context::instance();
}

inline known_functions& get_functions()
{
    if (1 == UE_BUILD_SHIPPING)
        return get_ctx().functions;
    else
		return get_ctx().wrappers;
}
