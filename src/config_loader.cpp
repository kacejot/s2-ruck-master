#include <windows.h>
#include <filesystem>
#include <fstream>
#include "config.h"

std::string get_ini_path()
{
    char buffer[MAX_PATH];
    GetModuleFileNameA(nullptr, buffer, MAX_PATH);

    std::filesystem::path p(buffer);
    p = p.parent_path() / "ruck_master.ini";
    return p.string();
}

bool save_ruck_config_ini(const ruck_master_config& cfg, preset_id id, const std::string& path)
{
    std::ofstream f(path);
    if (!f)
        return false;

	f << "[preset]\n";
	f << "id=" << preset_id_to_string(id) << "\n\n";

	if (CUSTOM != id)
        return true;

    f << "[rules]\n";
    for (auto& pair : cfg.sort_rules_order)
    {
        f << sort_rule_id_to_readable_string(pair.first) << "=" << (pair.second ? 1 : 0) << "\n";
    }
    f << "\n";
    f << "[item_type_priority]\n";
    for (size_t i = 0; i < cfg.item_types_priority.size(); i++)
    {
        f << i << "=" << item_type_id_to_string(cfg.item_types_priority[i]) << "\n";
    }
    f << "\n";
    f << "[flags]\n";
    f << "enable_logging=" << (cfg.enable_logging ? 1 : 0) << "\n";

    return true;
}

bool load_ruck_config_ini(ruck_master_config& cfg, preset_id& p_id, const std::string& path)
{
    std::ifstream f(path);
    if (!f)
        return false;

    std::string line;
    std::string section;

    while (std::getline(f, line))
    {
        if (line.empty() || line[0] == ';' || line[0] == '#')
            continue;

        if (line.front() == '[' && line.back() == ']')
        {
            section = line.substr(1, line.size() - 2);
            continue;
        }

        auto pos = line.find('=');
        if (pos == std::string::npos)
            continue;

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);
        if (section == "preset")
        {
            preset_id id = preset_id_from_string(value);
            if (id < PRESETS_TOTAL)
            {
                cfg = g_config_presets[id];
                p_id = id;
            }

			if (CUSTOM != id)
            {
                break; // no need to parse further
            }
        }
        else if (section == "rules")
        {
            sort_rule_id id = sort_rule_id_from_string(key);
            for (auto& pair : cfg.sort_rules_order)
            {
                if (pair.first == id)
                {
                    pair.second = (value == "1");
                    break;
                }
            }
        }
        else if (section == "item_type_priority")
        {
            int index = std::stoi(key);
            if (index >= 0 && index < (int)cfg.item_types_priority.size())
            {
                cfg.item_types_priority[index] = item_type_id_from_string(value);
            }
        }
        else if (section == "flags")
        {
            if (key == "enable_logging")
                cfg.enable_logging = (value == "1");
        }
    }

    return true;
}
