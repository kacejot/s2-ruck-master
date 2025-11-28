#include "hooking.h"
#include "MinHook.h"
#include "print.h"

hooking::~hooking()
{
    if (m_initialized)
        deinit();
}

hooking_result hooking::init()
{
    if (MH_OK != MH_Initialize())
        return hooking_result::MINHOOK_INIT_FAILED;

	m_initialized = true;

    return hooking_result::SUCCESS;
}

void hooking::deinit()
{
    for (auto& [_, hi] : m_hooks) {
        if (hi.address)
            MH_DisableHook((LPVOID)hi.address);

         hi.deleter(hi.hook);
    }

    MH_Uninitialize();
}

hooking_result hooking::install(hook_info& hi)
{
    if (MH_OK != MH_CreateHook((LPVOID)hi.address, (LPVOID)hi.detour, (LPVOID*)&hi.original))
        return hooking_result::CREATE_HOOK_FAILED;

    if (MH_OK != MH_EnableHook((LPVOID)hi.address))
        return hooking_result::ENABLE_HOOK_FAILED;

    return hooking_result::SUCCESS;
}
