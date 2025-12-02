#pragma once
#include <imgui.h>
#include "config.h"

template<typename T, size_t N>
bool DragReorder(std::array<T, N>& arr, int index, const char* id)
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

void RenderPresets();
void RenderLists();
void RenderHelpButton(const char* label, const char* text);
void RenderFlags();
bool IsRuleEnabled(sort_rule_id id);
