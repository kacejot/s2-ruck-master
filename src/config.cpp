#include "config.h"

std::array<ruck_master_config, PRESETS_TOTAL> g_config_presets =
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
	},
	{
	}
};
