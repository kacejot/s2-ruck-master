#include "config.h"
#include "presets.h"

#include <filesystem>
#include <sstream>
#include <iomanip>

#include <mini/ini.h>

std::string preset_id_to_string(preset_id id)
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

preset_id preset_id_from_string(const std::string& str)
{
    if (str == "Vanilla")
        return VANILLA;
    if (str == "Call of Pripyat")
        return COP;
    if (str == "Custom")
        return CUSTOM;
    return PRESETS_TOTAL;
}

config_manager g_config;

config_manager::config_manager()
{
    sorting = get_vanilla_preset();
    load_settings();
    load_cached_offsets();
}

config_manager::~config_manager()
{
    if (_is_dirty)
        save();
}

void config_manager::load_cached_offsets()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;

    if (!file.read(ini) || !ini.has("cached_offsets"))
    {
        has_cached_offsets = false;
        return;
    }

    const auto& section = ini["cached_offsets"];
    
    for (int i = 0; i < FUNCTIONS_TOTAL; i++)
    {
        known_function_id func_id = static_cast<known_function_id>(i);
        std::string key = known_function_id_to_string(func_id);
        
        if (section.has(key))
        {
            try {
                cached_offsets[func_id] = std::stoull(section.get(key), nullptr, 16);
            } catch (...) {
                has_cached_offsets = false;
                return;
            }
        }
        else
        {
            has_cached_offsets = false;
            return;
        }
    }
    
    has_cached_offsets = true;
    last_scan_failed = section.has("scan_failed") && section.get("scan_failed") == "1";
}

void config_manager::save_cached_offsets(const std::array<uintptr_t, FUNCTIONS_TOTAL>& offsets)
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;
    
    file.read(ini);
    
    for (int i = 0; i < FUNCTIONS_TOTAL; i++)
    {
        known_function_id func_id = static_cast<known_function_id>(i);
        std::string key = known_function_id_to_string(func_id);
        
        std::ostringstream oss;
        oss << "0x" << std::hex << std::uppercase << offsets[func_id];
        ini["cached_offsets"][key] = oss.str();
    }
    
    ini["cached_offsets"]["scan_failed"] = "0";
    
    file.write(ini);
    
    cached_offsets = offsets;
    has_cached_offsets = true;
    last_scan_failed = false;
}

void config_manager::mark_scan_failed()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;
    
    file.read(ini);
    ini["cached_offsets"]["scan_failed"] = "1";
    file.write(ini);
    
    last_scan_failed = true;
}

void config_manager::select_preset(preset_id id)
{
    if (id == _preset_id)
        return;

    load_preset(id);
    _preset_id = id;
    _is_dirty = true;
    save();
}

void config_manager::on_modified()
{
    if (_preset_id != CUSTOM)
        _preset_id = CUSTOM;
    
    _is_dirty = true;
    save();
}

void config_manager::save()
{
    save_settings();
    
    // Save custom preset to INI
    if (_preset_id == CUSTOM)
        save_custom_preset();
    
    _is_dirty = false;
}

void config_manager::save_settings()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;

    ini["settings"]["preset"] = preset_id_to_string(_preset_id);
    ini["settings"]["toggle_key"] = key_to_string(toggle_key);
    ini["settings"]["enable_logging"] = enable_logging ? "1" : "0";

    file.write(ini);
}

void config_manager::load_settings()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;

    if (!file.read(ini))
        return;

    if (ini.has("settings"))
    {
        const auto& settings = ini["settings"];

        if (settings.has("preset"))
        {
            preset_id id = preset_id_from_string(settings.get("preset"));
            if (id < PRESETS_TOTAL)
            {
                _preset_id = id;
                load_preset(id);
            }
        }

        if (settings.has("toggle_key"))
        {
            toggle_key = string_to_key(settings.get("toggle_key"));
        }

        if (settings.has("enable_logging"))
        {
            enable_logging = (settings.get("enable_logging") == "1");
        }
    }
}

void config_manager::load_preset(preset_id id)
{
    switch (id)
    {
    case VANILLA:
        sorting = get_vanilla_preset();
        break;
    case COP:
        sorting = get_cop_preset();
        break;
    case CUSTOM:
        load_custom_preset();
        break;
    default:
        sorting = get_vanilla_preset();
        break;
    }
}

void config_manager::save_custom_preset()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;
    
    // Load existing INI
    file.read(ini);

    // Save sort rules
    for (size_t i = 0; i < sorting.sort_rules_order.size(); i++)
    {
        const auto& [rule_id, enabled] = sorting.sort_rules_order[i];
        std::string key = "rule_" + std::to_string(i);
        ini["custom_rules"][key] = sort_rule_id_to_string(rule_id) + "," + (enabled ? "1" : "0");
    }

    // Save item type priorities
    for (size_t i = 0; i < sorting.item_types_priority.size(); i++)
    {
        std::string key = "type_" + std::to_string(i);
        ini["custom_item_priority"][key] = item_type_id_to_string(sorting.item_types_priority[i]);
    }

    file.write(ini);
}

void config_manager::load_custom_preset()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;

    if (!file.read(ini))
    {
        // No custom preset, use vanilla as fallback
        sorting = get_vanilla_preset();
        return;
    }

    // Load sort rules
    if (ini.has("custom_rules"))
    {
        const auto& rules = ini["custom_rules"];
        for (size_t i = 0; i < sorting.sort_rules_order.size(); i++)
        {
            std::string key = "rule_" + std::to_string(i);
            if (rules.has(key))
            {
                std::string value = rules.get(key);
                size_t comma = value.find(',');
                if (comma != std::string::npos)
                {
                    std::string rule_str = value.substr(0, comma);
                    std::string enabled_str = value.substr(comma + 1);
                    
                    sort_rule_id rule_id = sort_rule_id_from_string(rule_str);
                    bool enabled = (enabled_str == "1");
                    
                    if (rule_id != SORT_RULE_ID_INVALID)
                        sorting.sort_rules_order[i] = { rule_id, enabled };
                }
            }
        }
    }

    // Load item type priorities
    if (ini.has("custom_item_priority"))
    {
        const auto& types = ini["custom_item_priority"];
        for (size_t i = 0; i < sorting.item_types_priority.size(); i++)
        {
            std::string key = "type_" + std::to_string(i);
            if (types.has(key))
            {
                item_type_id type = item_type_id_from_string(types.get(key));
                if (type != ITEM_TYPE_UNKNOWN)
                    sorting.item_types_priority[i] = type;
            }
        }
    }
}

std::string config_manager::get_ini_path() const
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    std::filesystem::path p(buffer);
    p = p.parent_path() / "ruck_master.ini";
    return p.string();
}


std::string config_manager::key_to_string(UINT vk_code)
{
    switch (vk_code)
    {
    case VK_OEM_MINUS:  return "-";
    case VK_OEM_PLUS:   return "=";
    case VK_OEM_4:      return "[";
    case VK_OEM_6:      return "]";
    case VK_OEM_1:      return ";";
    case VK_OEM_7:      return "'";
    case VK_OEM_COMMA:  return ",";
    case VK_OEM_PERIOD: return ".";
    case VK_OEM_2:      return "/";
    case VK_OEM_5:      return "\\";
    case VK_OEM_3:      return "`";
    case VK_SPACE:      return "Space";
    case VK_TAB:        return "Tab";
    case VK_RETURN:     return "Enter";
    case VK_ESCAPE:     return "Escape";
    case VK_BACK:       return "Backspace";
    case VK_DELETE:     return "Delete";
    case VK_INSERT:     return "Insert";
    case VK_HOME:       return "Home";
    case VK_END:        return "End";
    case VK_PRIOR:      return "PageUp";
    case VK_NEXT:       return "PageDown";
    case VK_F1: case VK_F2: case VK_F3: case VK_F4:
    case VK_F5: case VK_F6: case VK_F7: case VK_F8:
    case VK_F9: case VK_F10: case VK_F11: case VK_F12:
        return "F" + std::to_string(vk_code - VK_F1 + 1);
    default:
        // A-Z, 0-9
        if (vk_code >= 'A' && vk_code <= 'Z')
            return std::string(1, (char)vk_code);
        if (vk_code >= '0' && vk_code <= '9')
            return std::string(1, (char)vk_code);
        // Fallback to numeric
        return std::to_string(vk_code);
    }
}

UINT config_manager::string_to_key(const std::string& str)
{
    if (str.empty())
        return VK_OEM_MINUS;

    // Single character keys
    if (str.length() == 1)
    {
        char c = str[0];
        if (c >= 'A' && c <= 'Z') return (UINT)c;
        if (c >= 'a' && c <= 'z') return (UINT)(c - 32); // Convert to uppercase
        if (c >= '0' && c <= '9') return (UINT)c;
        
        switch (c)
        {
        case '-': return VK_OEM_MINUS;
        case '=': return VK_OEM_PLUS;
        case '[': return VK_OEM_4;
        case ']': return VK_OEM_6;
        case ';': return VK_OEM_1;
        case '\'': return VK_OEM_7;
        case ',': return VK_OEM_COMMA;
        case '.': return VK_OEM_PERIOD;
        case '/': return VK_OEM_2;
        case '\\': return VK_OEM_5;
        case '`': return VK_OEM_3;
        }
    }

    // Named keys
    if (str == "Space") return VK_SPACE;
    if (str == "Tab") return VK_TAB;
    if (str == "Enter") return VK_RETURN;
    if (str == "Escape") return VK_ESCAPE;
    if (str == "Backspace") return VK_BACK;
    if (str == "Delete") return VK_DELETE;
    if (str == "Insert") return VK_INSERT;
    if (str == "Home") return VK_HOME;
    if (str == "End") return VK_END;
    if (str == "PageUp") return VK_PRIOR;
    if (str == "PageDown") return VK_NEXT;

    // F-keys
    if (str.length() >= 2 && str[0] == 'F')
    {
        int num = std::stoi(str.substr(1));
        if (num >= 1 && num <= 12)
            return VK_F1 + num - 1;
    }

    // Numeric fallback
    try
    {
        return (UINT)std::stoul(str);
    }
    catch (...)
    {
        return VK_OEM_MINUS;
    }
}
