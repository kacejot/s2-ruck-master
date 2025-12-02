#pragma once 
#include <string>
#include "local_types.h"

enum preset_id
	{
	VANILLA = 0,
	COP,
	CUSTOM,
	PRESETS_TOTAL
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

inline preset_id preset_id_from_string(const std::string& str)
{
	if (str == "Vanilla")
		return VANILLA;
	if (str == "Call of Pripyat")
		return COP;
	if (str == "Custom")
		return CUSTOM;
	return PRESETS_TOTAL;
}

struct ruck_master_config
{
	std::array<std::pair<sort_rule_id, bool>, RULES_TOTAL> sort_rules_order;
	std::array<item_type_id, ITEM_TYPES_TOTAL> item_types_priority;
	bool enable_logging;
};

extern std::array<ruck_master_config, PRESETS_TOTAL> g_config_presets;
