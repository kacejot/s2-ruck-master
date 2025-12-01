#pragma once
#include <imgui.h>

template <typename T>
void DragReorder(std::vector<T>& list, int index, const char* id)
{
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        ImGui::SetDragDropPayload(id, &index, sizeof(int));
        ImGui::Text("Move");
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(id))
        {
            int src_idx = *(int*)payload->Data;
            if (src_idx != index)
            {
                T temp = list[src_idx];
                if (src_idx < index)
                {
                    for (int i = src_idx; i < index; i++)
                        list[i] = list[i + 1];
                    list[index] = temp;
                }
                else
                {
                    for (int i = src_idx; i > index; i--)
                        list[i] = list[i - 1];
                    list[index] = temp;
                }
            }
        }
        ImGui::EndDragDropTarget();
    }
}

void RenderPresets();
void RenderLists();
void RenderHelpButton(const char* text);
void RenderBottomOptions();
bool IsRuleEnabled(const char* name);