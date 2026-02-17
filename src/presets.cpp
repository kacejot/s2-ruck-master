#include "presets.h"
#include "local_types.h"

const sorting_preset& get_vanilla_preset()
{
    static sorting_preset preset = {
        .sort_rules_order = {{
            {COMPARE_SIZE, false},
            {COMPARE_QUEST, true},
            {COMPARE_TYPE_PRIORITY, true},
            {COMPARE_AMMO_CALIBER, true},
            {COMPARE_SECONDARY_KEY, true},
            {COMPARE_NAME, true}
        }},
        .item_types_priority = {{
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
            MUTANT_PARTS
        }}
    };
    return preset;
}

const sorting_preset& get_cop_preset()
{
    static sorting_preset preset = {
        .sort_rules_order = {{
            {COMPARE_SIZE, true},
            {COMPARE_QUEST, false},
            {COMPARE_TYPE_PRIORITY, true},
            {COMPARE_AMMO_CALIBER, true},
            {COMPARE_SECONDARY_KEY, true},
            {COMPARE_NAME, true}
        }},
        .item_types_priority = {{
            WEAPON,
            ARMOR,
            WEAPON_MOD,
            AMMO,
            NVG,
            DETECTOR,
            PDA,
            ARTIFACT,
            MUTANT_PARTS,
            CONSUMABLES, 
            MISC,
        }}
    };
    return preset;
}
