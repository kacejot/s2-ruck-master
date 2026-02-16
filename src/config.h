#pragma once 
#include <Windows.h>
#include <string>
#include "local_types.h"

enum preset_id
{
    VANILLA = 0,
    COP,
    CUSTOM,
    PRESETS_TOTAL
};

std::string preset_id_to_string(preset_id id);
preset_id preset_id_from_string(const std::string& str);

// Preset data - sorting rules configuration
struct sorting_preset
{
    std::array<std::pair<sort_rule_id, bool>, RULES_TOTAL> sort_rules_order;
    std::array<item_type_id, ITEM_TYPES_TOTAL> item_types_priority;
};

class config_manager
{
public:
    sorting_preset sorting;
    bool enable_logging = false;
    UINT toggle_key = VK_OEM_MINUS;

    config_manager();
    ~config_manager();

    preset_id current_preset() const { return _preset_id; }

    void select_preset(preset_id id);
    void on_modified();
    void save();

private:
    void load_settings();
    void save_settings();
    bool load_preset_from_json(preset_id id, sorting_preset& out);
    
    std::string get_ini_path() const;
    std::string get_preset_path(preset_id id) const;
    std::string get_presets_dir() const;

    // Key serialization
    static std::string key_to_string(UINT vk_code);
    static UINT string_to_key(const std::string& str);

    preset_id _preset_id = VANILLA;
    bool _is_dirty = false;
};

extern config_manager g_config;
