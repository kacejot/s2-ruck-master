#pragma once
#include <String/StringType.hpp>

namespace Hooking
{
	enum Result
	{
		Success,
		MinHookInitFailed,
		GetModuleFailed,
		ComparerHookFailed,
	};

	RC::StringType ResultToString(Result result);

	Result InitializeHooks();
	void UninitializeHooks();
}
