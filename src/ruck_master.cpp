#include <windows.h>
#include <memory>

#include <overlay.h>
#include <imgui.h>
#include <backends/imgui_impl_dx12.h>

#include "comparator_hook.h"
#include "config.h"
#include "menu.h"

class ruck_master;
std::unique_ptr<ruck_master> g_ruck_master;

static void render_callback(IDXGISwapChain3* swap_chain, UINT sync_interval, UINT flags);
static void font_config_callback();

class ruck_master
{
public:
    ruck_master()
    {
        uintptr_t game_base = (uintptr_t)GetModuleHandleA(nullptr);
        init_comparator(game_base);

        if (g_config.enable_logging)
            overlay::enable_logging("ruck_master_log.txt");

        overlay::set_toggle_key(g_config.toggle_key);
        overlay::set_render_callback(render_callback);
        overlay::set_font_config_callback(font_config_callback);
        overlay::init();
    }

    ~ruck_master()
    {
        overlay::deinit();
        deinit_comparator();
    }

    void render(IDXGISwapChain3*, UINT, UINT)
    {
        if (!m_ui_configured)
        {
            ImGuiStyle& style = ImGui::GetStyle();
            //style.ScaleAllSizes(1.3f);

            m_ui_configured = true;
        }

        ImGui::SetNextWindowSize(ImVec2(600, 650), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowPos(ImVec2(50, 50), ImGuiCond_FirstUseEver);

        ImGui::Begin("Ruck Master");
        m_menu.render();
        ImGui::End();
    }

private:
    menu m_menu;
    bool m_ui_configured = false;
};

static void font_config_callback()
{
    ImGuiIO& io = ImGui::GetIO();
    io.IniFilename = nullptr;
    
    io.Fonts->Clear();
    
    ImFontConfig font_cfg;
    font_cfg.OversampleH = 2;
    font_cfg.OversampleV = 2;
    
    ImFont* font = io.Fonts->AddFontFromFileTTF(
        "C:\\Windows\\Fonts\\consola.ttf",
        20.0f,
        &font_cfg,
        io.Fonts->GetGlyphRangesCyrillic()
    );
    
    if (!font)
    {
        font_cfg.SizePixels = 20.0f;
        io.Fonts->AddFontDefault(&font_cfg);
    }
}

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
