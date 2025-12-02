#pragma once
#include <windows.h>
#include "local_types.h"
#include "config.h"
#include "context_function_wrapper.h"
#include "config_loader.h"

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
    preset_id current_config_preset;
    known_functions functions;
    uintptr_t game_base;
	bool config_is_dirty;

    global_context()
    {
        
        load_ruck_config_ini(
            config,
            current_config_preset,
            get_ini_path()
		);

        game_base = (uintptr_t)GetModuleHandleA(nullptr);
        functions =
        {
            WRAP_WITH_LOG(get_global_state_t,         game_base + known_function_offsets[GET_GLOBAL_STATE]),
            WRAP_WITH_LOG(get_item_by_descriptor_t,   game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]),
            WRAP_WITH_LOG(get_item_metadata_t,        game_base + known_function_offsets[GET_ITEM_METADATA]),
            WRAP_WITH_LOG(get_weapon_from_item_t,     game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]),
            WRAP_WITH_LOG(get_secondary_sort_key_t,   game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]),
            WRAP_WITH_LOG(get_item_name_t,            game_base + known_function_offsets[GET_ITEM_NAME]),
            WRAP_WITH_LOG(compare_names_t,            game_base + known_function_offsets[COMPARE_ITEM_NAMES]),
            WRAP_WITH_LOG(free_s2string_t,            game_base + known_function_offsets[FREE_ITEM_NAME]),
        };
    }

    void switch_preset(preset_id id)
    {
        if (id == current_config_preset)
			return;

        if (id < PRESETS_TOTAL)
        {
            if (CUSTOM != id)
                config = g_config_presets[id];
            

            current_config_preset = id;
        }

        save_ruck_config_ini(
            config,
            current_config_preset,
            get_ini_path()
        );
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
