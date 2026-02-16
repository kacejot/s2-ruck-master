#include "config.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include <picojson.h>
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
    sorting_preset default_preset;
    if (load_preset_from_json(VANILLA, default_preset))
        sorting = default_preset;
    
    load_settings();
}

config_manager::~config_manager()
{
    if (_is_dirty)
        save();
}

void config_manager::select_preset(preset_id id)
{
    if (id == _preset_id)
        return;

    if (id != CUSTOM)
    {
        sorting_preset preset;
        if (load_preset_from_json(id, preset))
            sorting = preset;
    }

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
    _is_dirty = false;
}

void config_manager::save_settings()
{
    mINI::INIFile file(get_ini_path());
    mINI::INIStructure ini;

    if (!file.read(ini))
        return;

    ini["settings"]["preset"] = preset_id_to_string(_preset_id);

    if (_preset_id == CUSTOM)
    {
        for (const auto& [rule_id, enabled] : sorting.sort_rules_order)
        {
            ini["custom_rules"][sort_rule_id_to_string(rule_id)] = enabled ? "1" : "0";
        }

        for (size_t i = 0; i < sorting.item_types_priority.size(); i++)
        {
            ini["custom_item_priority"][std::to_string(i)] = item_type_id_to_string(sorting.item_types_priority[i]);
        }
    }

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
                if (id != CUSTOM)
                {
                    sorting_preset preset;
                    if (load_preset_from_json(id, preset))
                        sorting = preset;
                }
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

    if (_preset_id == CUSTOM)
    {
        if (ini.has("custom_rules"))
        {
            const auto& rules = ini["custom_rules"];
            for (auto& [rule_id, enabled] : sorting.sort_rules_order)
            {
                std::string key = sort_rule_id_to_string(rule_id);
                if (rules.has(key))
                {
                    enabled = (rules.get(key) == "1");
                }
            }
        }

        if (ini.has("custom_item_priority"))
        {
            const auto& priority = ini["custom_item_priority"];
            for (size_t i = 0; i < sorting.item_types_priority.size(); i++)
            {
                std::string key = std::to_string(i);
                if (priority.has(key))
                {
                    item_type_id type = item_type_id_from_string(priority.get(key));
                    if (type != ITEM_TYPE_UNKNOWN)
                    {
                        sorting.item_types_priority[i] = type;
                    }
                }
            }
        }
    }
}

bool config_manager::load_preset_from_json(preset_id id, sorting_preset& out)
{
    if (id == CUSTOM)
        return false;

    std::ifstream f(get_preset_path(id));
    if (!f)
        return false;

    std::string json_str((std::istreambuf_iterator<char>(f)), std::istreambuf_iterator<char>());

    picojson::value root;
    std::string err = picojson::parse(root, json_str);
    if (!err.empty() || !root.is<picojson::object>())
        return false;

    const auto& obj = root.get<picojson::object>();

    // Parse rules array
    auto rules_it = obj.find("rules");
    if (rules_it != obj.end() && rules_it->second.is<picojson::array>())
    {
        const auto& rules = rules_it->second.get<picojson::array>();
        for (size_t i = 0; i < rules.size() && i < RULES_TOTAL; i++)
        {
            if (!rules[i].is<picojson::object>())
                continue;

            const auto& rule_obj = rules[i].get<picojson::object>();

            auto id_it = rule_obj.find("id");
            auto enabled_it = rule_obj.find("enabled");

            if (id_it != rule_obj.end() && id_it->second.is<std::string>())
            {
                sort_rule_id rule_id = sort_rule_id_from_string(id_it->second.get<std::string>());
                bool enabled = (enabled_it != rule_obj.end() && enabled_it->second.is<bool>()) 
                    ? enabled_it->second.get<bool>() 
                    : true;

                if (rule_id != SORT_RULE_ID_INVALID)
                {
                    out.sort_rules_order[i] = { rule_id, enabled };
                }
            }
        }
    }

    // Parse item_types_priority array
    auto types_it = obj.find("item_types_priority");
    if (types_it != obj.end() && types_it->second.is<picojson::array>())
    {
        const auto& types = types_it->second.get<picojson::array>();
        for (size_t i = 0; i < types.size() && i < ITEM_TYPES_TOTAL; i++)
        {
            if (!types[i].is<std::string>())
                continue;

            item_type_id type = item_type_id_from_string(types[i].get<std::string>());
            if (type != ITEM_TYPE_UNKNOWN)
            {
                out.item_types_priority[i] = type;
            }
        }
    }

    return true;
}

std::string config_manager::get_ini_path() const
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    std::filesystem::path p(buffer);
    p = p.parent_path() / "ruck_master.ini";
    return p.string();
}

std::string config_manager::get_presets_dir() const
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    std::filesystem::path p(buffer);
    p = p.parent_path() / "ruck_master_presets";
    return p.string();
}

std::string config_manager::get_preset_path(preset_id id) const
{
    std::string filename;
    switch (id)
    {
    case VANILLA:
        filename = "vanilla.json";
        break;
    case COP:
        filename = "cop.json";
        break;
    default:
        return "";
    }

    return (std::filesystem::path(get_presets_dir()) / filename).string();
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
