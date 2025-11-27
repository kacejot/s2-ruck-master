#include <iostream>
#include <Mod/CppUserModBase.hpp>
#include "hooking.h"

class RuckMaster : public CppUserModBase
{
public:
    RuckMaster()
    {
        ModName = STR("s2-ruck-master");
        ModVersion = STR("0.1");
        ModDescription = STR("s2-ruck-master is a S.T.A.L.K.E.R. 2 mod that enhances inventory sorting and provides fully customizable item-placement rules through an ImGui-based interface");
        ModAuthors = STR("kacejot");
        
		if (auto result = Hooking::InitializeHooks(); result == Hooking::Result::Success)
            Output::send<LogLevel::Verbose>(STR("s2-ruck-master init success\n"));
        else 
            Output::send<LogLevel::Error>(STR("s2-ruck-master init fail: %s\n"), Hooking::ResultToString(result).c_str());
    }

    ~RuckMaster() override
    {
		Hooking::UninitializeHooks();
    }
};

extern "C"
{
    __declspec(dllexport) CppUserModBase* start_mod()
    {
        return new RuckMaster();
    }

    __declspec(dllexport) void uninstall_mod(RC::CppUserModBase* mod)
    {
        delete mod;
    }
}
