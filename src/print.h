#pragma once
#include <DynamicOutput/Output.hpp>


static const RC::StringType g_fmt = STR("[s2-ruck-master]: {}\n");;

#define CHECK(expr)                                                             \
do {                                                                            \
    if (hooking_result _r = (expr); _r != hooking_result::SUCCESS) {            \
        RC::Output::send<RC::LogLevel::Error>(g_fmt, to_string(_r));            \
        return;                                                                 \
    }                                                                           \
} while(0)


#define LOG(level, ...)                                                         \
    RC::Output::send<RC::LogLevel::level>(g_fmt, ##__VA_ARGS__);
