#pragma once
#include "config.h"

std::string get_ini_path();
bool save_ruck_config_ini(const ruck_master_config& cfg, preset_id id, const std::string& path);
bool load_ruck_config_ini(ruck_master_config& cfg, preset_id& id, const std::string& path);
