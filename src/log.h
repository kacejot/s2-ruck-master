#pragma once
#include <fstream>
#include <format>
#include "config.h"

inline std::ofstream g_log("ruck_master_log.txt", std::ios::out | std::ios::trunc);

#define LOG(fmt, ...)                                                    \
    do {                                                                 \
        if (g_config.enable_logging && g_log) {                          \
            auto _msg = std::format(fmt, ##__VA_ARGS__);                 \
            g_log << _msg << std::endl;                                  \
            g_log.flush();                                               \
        }                                                                \
    } while (0)