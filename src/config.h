#pragma once 
#include <string>
#include "local_types.h"

enum preset_id
	{
	VANILLA = 0,
	COP,
	CUSTOM,   // TODO: do not load from memory - load from save file
	PRESETS_TOTAL
};

struct ruck_master_config
{
	std::array<std::pair<sort_rule_id, bool>, RULES_TOTAL> sort_rules_order;
	std::array<item_type_id, ITEM_TYPES_TOTAL> item_types_priority;
	preset_id current_preset;
	bool enable_logging;
	bool allow_editing;
};

inline std::string preset_id_to_string(preset_id id)
{
	switch (id)
	{
	case VANILLA:
		return "Vanilla";
	case COP:
		return "Call of Pripyat";
	case CUSTOM:
		return "Custom";
	default:
		return "Unknown";
	}
}

constexpr std::array<ruck_master_config, PRESETS_TOTAL> g_config_presets =
{
	ruck_master_config{
		{
			std::pair<sort_rule_id, bool>
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
			PDA,
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
	{
		{
			std::pair<sort_rule_id, bool>
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
			PDA,
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
	{
		{
			std::pair<sort_rule_id, bool>
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
			PDA,
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
		true
	}
};
