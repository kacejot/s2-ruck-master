#pragma once
#include "local_types.h"
#include "config.h"

struct known_functions
{
	get_global_state_t          get_global_object_pool;
	get_item_by_descriptor_t    get_item_by_descriptor;
	get_item_metadata_t         get_item_metadata;
	get_weapon_from_item_t      get_weapon_from_item;
	get_secondary_sort_key_t    get_secondary_sort_key;
	get_item_name_t             get_item_name;
	compare_names_t             compare_names;
	free_s2string_t             free_s2string;
};

struct context
{
	ruck_master_config config;
    preset_id current_config_preset;
    known_functions functions;
    uintptr_t game_base;
	bool config_is_dirty;

    context();
    void switch_preset(preset_id id);
};
