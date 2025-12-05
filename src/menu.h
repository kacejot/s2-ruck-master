#pragma once
#include <imgui.h>
#include <array>

struct context;
enum sort_rule_id;

class menu
{
public:
    menu(context& ctx);
    void render();

private:
    void render_presets();
    void render_lists();
    void render_help_button(const char* label, const char* text);
    void render_flags();
    bool is_rule_enabled(sort_rule_id id);

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

private:
    context& m_ctx;
};
