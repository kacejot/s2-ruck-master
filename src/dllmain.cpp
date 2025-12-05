#include <iostream>
#include <windows.h>

#include "comparator_hook.h"
#include "context.h"
#include "hooking.h"
#include "menu.h"

class ruck_master
{
public:
    ruck_master():
        m_context{},
        m_hooking{ m_context },
		m_comparator_hook{ m_context },
        m_menu{ m_context }
    {
        // TODO: check if success
        // TODO: add logging (file & stdout)
        m_hooking.init();
        m_hooking.add_hook<COMPARATOR>(m_comparator_hook);
    }

private:
    context m_context;
	hooking m_hooking;
    comparator_hook m_comparator_hook;
    menu m_menu;
};

ruck_master* g_ruck_master = nullptr;

extern "C" void init_plugin(HMODULE h_module)
{
    UNREFERENCED_PARAMETER(h_module);
    g_ruck_master = new ruck_master();
}

extern "C" void deinit_plugin()
{
    delete g_ruck_master;
}
