#pragma once
#include <DynamicOutput/Output.hpp>
#include "constants.h"

static inline std::wstring_view extract_name(std::string_view full) {
    size_t pos = full.rfind("_t");
    if (pos == std::string_view::npos)
        return std::wstring(full.begin(), full.end());
    return std::wstring(full.begin(), full.begin() + pos);
}

#define WRAP_WITH_LOG(signature_t, address)                                      \
    ([orig_fn = reinterpret_cast<signature_t>(address), this](auto&&... args) {  \
        if (config.enable_logging) {                                             \
            constexpr std::string_view full_name = #signature_t;                 \
            std::wstring name = std::wstring(extract_name(full_name));           \
                                                                                 \
            std::wstringstream ss;                                               \
            ss << name << L"(";                                                  \
            bool first = true;                                                   \
            ((ss << (first ? (first = false, L"") : L", ")                       \
                    << std::format(L"0x{:x}", (uintptr_t)args)), ...);           \
            ss << L")";                                                          \
            using namespace RC;                                                  \
            Output::send<LogLevel::Verbose>(g_fmt, ss.str());                    \
        }                                                                        \
        return orig_fn(std::forward<decltype(args)>(args)...);                   \
    })
