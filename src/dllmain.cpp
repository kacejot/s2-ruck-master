#include <iostream>
#include <windows.h>
#include <Mod/CppUserModBase.hpp>

#include "global_context.h"
#include "comparer_hook.h"
#include "hooking.h"
#include "print.h"

class RuckMaster : public CppUserModBase
{
public:
    RuckMaster()
    {
        ModName = STR("s2-ruck-master");
        ModVersion = STR("0.1");
        ModDescription = STR("s2-ruck-master is a S.T.A.L.K.E.R. 2 mod that enhances inventory sorting and provides fully customizable item-placement rules through an ImGui-based interface");
        ModAuthors = STR("kacejot");

        auto& game_base = global_context::instance().game_base;
        game_base = (uintptr_t)GetModuleHandleA(nullptr);
        if (NULL == game_base) {
            LOG(Error, STR("failed to get game image base address"));
        }

        CHECK(m_hooking.init(game_base));
        CHECK(m_hooking.add_hook<COMPARER>(Comparer));

        LOG(Verbose, STR("init success"));
    }

private:
	hooking m_hooking;
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
