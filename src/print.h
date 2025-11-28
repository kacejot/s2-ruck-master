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

static inline std::wstring_view extract_name(std::string_view full) {
    size_t pos = full.rfind('.');
    if (pos == std::string_view::npos)
        return std::wstring(full.begin(), full.end());
    return std::wstring(full.begin() + pos + 1, full.end());
}

#define WRAP_FN(fn)                                                          \
    ([orig_fn = (fn)](auto&&... args) {                                      \
        constexpr std::string_view full_name = #fn;                          \
        std::wstring name = std::wstring(extract_name(full_name));           \
                                                                             \
        std::wstringstream ss;                                               \
        ss << name << L"(";                                                  \
        bool first = true;                                                   \
        ((ss << (first ? (first = false, L"") : L", ")                       \
             << std::format(L"0x{:x}", (uintptr_t)args)), ...);              \
        ss << L")";                                                          \
        LOG(Verbose, ss.str());                                              \
                                                                             \
        return orig_fn(std::forward<decltype(args)>(args)...);               \
    })

#define LOG(level, ...)                                                      \
    RC::Output::send<RC::LogLevel::level>(g_fmt, ##__VA_ARGS__);
