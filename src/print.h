#pragma once
#include <DynamicOutput/Output.hpp>
#include "constants.h"
#include "global_context.h"

#define CHECK(expr)                                                              \
do {                                                                             \
    if (hooking_result _r = (expr); _r != hooking_result::SUCCESS) {             \
        RC::Output::send<RC::LogLevel::Error>(g_fmt, to_string(_r));             \
        return;                                                                  \
    }                                                                            \
} while(0)

#define LOG(level, fmt, ...)                                                     \
    if (get_ctx().config.enable_logging)                                         \
        RC::Output::send<RC::LogLevel::level>(std::format(g_fmt, fmt), ##__VA_ARGS__);
