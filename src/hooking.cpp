#include "hooking.h"
#include "MinHook.h"
#include <DynamicOutput/Output.hpp>

using namespace Hooking;

using ComparerType = bool(*)(void*, void*);

uintptr_t g_comparerAddress = 0;
uintptr_t g_comparerOffset = 0x01240FC0;
ComparerType g_originalComparer = nullptr;

bool ComparerHook(void* a, void* b)
{
	RC::Output::send<RC::LogLevel::Verbose>(STR("SORT IS CALLED\n"));
	return g_originalComparer(a, b);
}

RC::StringType Hooking::ResultToString(Result result)
{
	switch (result)
	{
	case Result::Success:
		return STR("success");
	case Result::MinHookInitFailed:
		return STR("MinHook initialization failed");
	case Result::GetModuleFailed:
		return STR("couldn't retrieve base address of the game");
	case Result::ComparerHookFailed:
		return STR("comparer hook failed");
	}
}

Result Hooking::InitializeHooks()
{
	if (MH_OK != MH_Initialize())
	{
		return Result::MinHookInitFailed;
	}
	
	HMODULE exeBase = GetModuleHandleA(NULL);
	if (exeBase == NULL)
	{
		return Result::GetModuleFailed;
	}

	g_comparerAddress = (uintptr_t)exeBase + g_comparerOffset;
	if (MH_OK != MH_CreateHook((LPVOID)g_comparerAddress, &ComparerHook, (LPVOID*)&g_originalComparer))
	{
		return Result::ComparerHookFailed;
	}

	if (MH_OK != MH_EnableHook((LPVOID)g_comparerAddress))
	{
		return Result::ComparerHookFailed;
	}
	
	return Result::Success;
}

void Hooking::UninitializeHooks()
{
	if (MH_OK != MH_DisableHook(&g_comparerAddress))
	{
		return;
	}

	MH_Uninitialize();
}
