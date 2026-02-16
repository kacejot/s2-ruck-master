#include <windows.h>
#include <memory>

#include <overlay.h>
#include <imgui.h>

#include "comparator_hook.h"
#include "config.h"
#include "menu.h"

class ruck_master;
std::unique_ptr<ruck_master> g_ruck_master;

static void render_callback(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags);

class ruck_master
{
public:
    ruck_master()
    {
        uintptr_t game_base = (uintptr_t)GetModuleHandleA(nullptr);
        init_comparator(game_base);

        overlay::init();
        overlay::set_toggle_key(g_config.toggle_key);
        overlay::set_render_callback(render_callback);
    }

    ~ruck_master()
    {
        overlay::deinit();
    }

    void render(IDXGISwapChain3*, UINT, UINT)
    {
        ImGui::Begin("Ruck Master");
        m_menu.render();
        ImGui::End();
    }

private:
    menu m_menu;
};

static void render_callback(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags)
{
    if (g_ruck_master)
        g_ruck_master->render(swap_chain, sync_interval, flags);
}

extern "C" void init_plugin(HMODULE h_module)
{
    UNREFERENCED_PARAMETER(h_module);
    g_ruck_master = std::make_unique<ruck_master>();
}

extern "C" void deinit_plugin()
{
    g_ruck_master.reset();
}
