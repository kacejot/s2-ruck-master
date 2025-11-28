#pragma once
#include <String/StringType.hpp>

enum class hooking_result
{
	SUCCESS,
	MINHOOK_INIT_FAILED,
	CREATE_HOOK_FAILED,
	ENABLE_HOOK_FAILED,
};

inline RC::StringType to_string(hooking_result result)
{
    switch (result)
    {
    case hooking_result::SUCCESS:
        return STR("success");
    case hooking_result::MINHOOK_INIT_FAILED:
        return STR("MinHook initialization failed");
    case hooking_result::CREATE_HOOK_FAILED:
        return STR("failed to create hook");
    case hooking_result::ENABLE_HOOK_FAILED:
        return STR("failed to enable hook");
    default:
        return STR("unknown error");
    }
}
