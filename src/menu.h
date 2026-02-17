#pragma once
#include <imgui.h>
#include <array>
#include <string>
#include <atomic>

#include "local_types.h"

class menu
{
public:
    void render();

private:
    void render_presets();
    void render_lists();
    void render_error(const std::string& error);
    void render_help_button(const char* label, const char* text);
    bool is_rule_enabled(sort_rule_id id);
    
    std::atomic<bool> m_scanning = false;
    std::atomic<float> m_scan_progress = 0.0f;

    template<typename T, size_t N>
    bool drag_reorder(std::array<T, N>& arr, int index, const char* id)
    {
        if (ImGui::BeginDragDropSource())
        {
            ImGui::SetDragDropPayload(id, &index, sizeof(int));
            ImGui::Text("Move");
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
            {
                int src = *(int*)payload->Data;
                if (src != index)
                {
                    auto temp = arr[src];
                    if (src < index)
                    {
                        for (int i = src; i < index; i++)
                            arr[i] = arr[i + 1];
                        arr[index] = temp;
                    }
                    else
                    {
                        for (int i = src; i > index; i--)
                            arr[i] = arr[i - 1];
                        arr[index] = temp;
                    }

                    return true;
                }
            }
            ImGui::EndDragDropTarget();
        }

        return false;
    }
};
