#pragma once 
#include "local_types.h"

struct ruck_master_config
{
	std::list<std::pair<sort_rule_id, bool>> sort_rules_order;
	std::list<item_type_id> item_types_priority;
	bool enable_logging;
	bool allow_editing;
};

enum preset_id
	{
	VANILLA = 0,
	COP,
	CUSTOM,
	PRESETS_TOTAL
};



std::array<ruck_master_config, PRESETS_TOTAL> g_config_presets =
{
	ruck_master_config{
		{
			{ COMPARE_SIZE, false },
			{ COMPARE_QUEST, true },
			{ COMPARE_TYPE_PRIORITY, true },
			{ COMPARE_AMMO_CALIBER, true },
			{ COMPARE_SECONDARY_KEY, true },
			{ COMPARE_NAME, true },
		},
		{
			CONSUMABLES,
			AMMO,
			UNDEFINED_1,
			NVG,
			WEAPON_MOD,
			DETECTOR,
			ARTIFACT,
			WEAPON,
			ARMOR,
			MISC,
			MUTANT_PARTS,
		},

		false, // enable_logging
		false  // allow_editing
	},
	ruck_master_config{
		{
			{ COMPARE_SIZE, true },
			{ COMPARE_QUEST, false },
			{ COMPARE_TYPE_PRIORITY, true },
			{ COMPARE_AMMO_CALIBER, true },
			{ COMPARE_SECONDARY_KEY, true },
			{ COMPARE_NAME, true },
		},
		{
			CONSUMABLES,
			AMMO,
			UNDEFINED_1,
			NVG,
			WEAPON_MOD,
			DETECTOR,
			ARTIFACT,
			WEAPON,
			ARMOR,
			MISC,
			MUTANT_PARTS,
		},

		false,
		false
	},
	ruck_master_config{
		{
			{ COMPARE_SIZE, true },
			{ COMPARE_QUEST, true },
			{ COMPARE_TYPE_PRIORITY, true },
			{ COMPARE_AMMO_CALIBER, true },
			{ COMPARE_SECONDARY_KEY, true },
			{ COMPARE_NAME, true },
		},
		{
			CONSUMABLES,
			AMMO,
			UNDEFINED_1,
			NVG,
			WEAPON_MOD,
			DETECTOR,
			ARTIFACT,
			WEAPON,
			ARMOR,
			MISC,
			MUTANT_PARTS,
		},

		false, // enable_logging
		true  // allow_editing
	}
};

// TODO: load/unload config from file