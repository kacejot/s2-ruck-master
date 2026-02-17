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
    
    std::array<uintptr_t, FUNCTIONS_TOTAL> cached_offsets = {};
    bool has_cached_offsets = false;
    bool last_scan_failed = false;

    config_manager();
    ~config_manager();

    preset_id current_preset() const { return _preset_id; }

    void select_preset(preset_id id);
    void on_modified();
    void save();
    
    void load_cached_offsets();
    void save_cached_offsets(const std::array<uintptr_t, FUNCTIONS_TOTAL>& offsets);
    void mark_scan_failed();

private:
    void load_settings();
    void save_settings();
    void load_preset(preset_id id);
    void save_custom_preset();
    void load_custom_preset();
    
    std::string get_ini_path() const;

    static std::string key_to_string(UINT vk_code);
    static UINT string_to_key(const std::string& str);

    preset_id _preset_id = VANILLA;
    bool _is_dirty = false;
};

extern config_manager g_config;
