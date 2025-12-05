#include "context.h"

#include <windows.h>
#include "config_loader.h"

context::context()
{
    load_ruck_config_ini(
        config,
        current_config_preset,
        get_ini_path()
    );

    game_base = (uintptr_t)GetModuleHandleA(nullptr);
    functions =
    {
        get_global_state_t(game_base + known_function_offsets[GET_GLOBAL_STATE]),
        get_item_by_descriptor_t(game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]),
        get_item_metadata_t(game_base + known_function_offsets[GET_ITEM_METADATA]),
        get_weapon_from_item_t(game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]),
        get_secondary_sort_key_t(game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]),
        get_item_name_t(game_base + known_function_offsets[GET_ITEM_NAME]),
        compare_names_t(game_base + known_function_offsets[COMPARE_ITEM_NAMES]),
        free_s2string_t(game_base + known_function_offsets[FREE_ITEM_NAME]),
    };
}

void context::switch_preset(preset_id id)
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
