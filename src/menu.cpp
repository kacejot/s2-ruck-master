#include "menu.h"
#include "global_context.h"

void RenderPresets()
{
    ImGui::Text("Presets:");
    ImGui::SetNextItemWidth(250.0f);

	preset_id preset_index = get_ctx().config.current_preset;

    if (ImGui::BeginCombo("##preset_selector", preset_id_to_string(preset_index).c_str()))
    {
        for (int i = 0; i < PRESETS_TOTAL; i++)
        {
            bool selected = (preset_index == i);
            if (ImGui::Selectable(preset_id_to_string((preset_id)i).c_str(), selected))
                preset_index = (preset_id)i;
            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void RenderLists()
{
    bool right_list_enabled = IsRuleEnabled("prioritize type");

    ImGui::Columns(2, nullptr, false);

    ImGui::Text("Rules");
    ImGui::SameLine();
    RenderHelpButton("Rules define the order in which items\nare compared inside the sorting algorithm.");

    ImGui::BeginChild("rules_list", ImVec2(0, 300), true);

    for (int i = 0; i < RULES_TOTAL; i++)
    {
        auto& r = rules[i];

        // Gray out disabled items
        if (!r.enabled)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1.0));

        bool clicked = ImGui::Selectable(r.name.c_str(), false);

        DragReorder(rules, i, "RULE_DRAG");

        if (!r.enabled)
            ImGui::PopStyleColor();

        // Toggle on click
        if (clicked)
            r.enabled = !r.enabled;
    }

    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::Text("Priority:");
    ImGui::SameLine();
    RenderHelpButton("Priority determines category precedence\nin final sorting.");

    // If disabled — fade whole region
    if (!right_list_enabled)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);

    ImGui::BeginChild("priority_list", ImVec2(0, 300), true);

    for (int i = 0; i < priority.size(); i++)
    {
        ImGui::Selectable(priority[i].c_str(), false);
        if (right_list_enabled)
            DragReorder(priority, i, "PRIORITY_DRAG");
    }

    ImGui::EndChild();

    if (!right_list_enabled)
        ImGui::PopStyleVar();

    ImGui::Columns(1);
}

void RenderHelpButton(const char* text)
{
    if (ImGui::SmallButton("?"))
    {
        // Could open popup in future
    }
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", text);
}

void RenderBottomOptions()
{
    ImGui::Checkbox("use original comparator", &use_original_comparator);
    ImGui::Checkbox("enable logs", &enable_logs);
}

bool IsRuleEnabled(const char* name)
{
    for (auto& r : rules)
        if (r.name == name)
            return r.enabled;
    return true;
}
