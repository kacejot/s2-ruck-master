#include "menu.h"
#include "config.h"
#include <overlay.h>

void menu::render()
{
    render_presets();
    ImGui::Separator();
    render_lists();
    ImGui::Separator();
}

void menu::render_presets()
{
    ImGui::Text("Presets:");

    ImGui::SetNextItemWidth(300);
    preset_id current_preset = g_config.current_preset();
    std::string preset_name_str = preset_id_to_string(current_preset);

    if (ImGui::BeginCombo("##preset", preset_name_str.c_str()))
    {
        for (int i = 0; i < PRESETS_TOTAL; i++)
        {
            preset_id p_id = (preset_id)i;
            bool selected = (current_preset == p_id);
            if (ImGui::Selectable(preset_id_to_string(p_id).c_str(), selected))
            {
                g_config.select_preset(p_id);
            }

            if (selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void menu::render_lists()
{
    bool right_enabled = is_rule_enabled(COMPARE_TYPE_PRIORITY);

    ImGui::Columns(2);
    ImGui::Text("Rules");
    ImGui::SameLine();
    render_help_button("?##rules_small_button", 
        "Rule at the top of the list has the highest priority.\n"
        "If rule considers two items equal, next rule in the list will process them.\n"
        "Drag and drop the rules to change their priority.\n"
        "Click on the rules to toggle them.\n\n"
        "NOTE: I'm not sure what \"Sort by secondary key\" is. "
        "I'm most certain that it is a durability for weapons.");

    ImGui::BeginChild("rules_list", ImVec2(0, 500), true);

    for (int i = 0; i < RULES_TOTAL; i++)
    {
        auto& [rule_id, rule_enabled] = g_config.sorting.sort_rules_order[i];

        if (!rule_enabled)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5f, 0.5f, 0.5f, 1.0f));

        bool clicked = ImGui::Selectable(sort_rule_id_to_readable_string(rule_id).c_str(), false);

        if (!rule_enabled)
            ImGui::PopStyleColor();

        if (drag_reorder(g_config.sorting.sort_rules_order, i, "RULE_DRAG"))
            g_config.on_modified();

        if (clicked)
        {
            rule_enabled = !rule_enabled;
            g_config.on_modified();
        }
    }

    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::Text("Item type priorities");
    ImGui::SameLine();
    render_help_button("?##type_priorities_small_button", 
        "NOTE: This list works only if \"Prioritize by type\" rule is active.\n\n"
        "Item type priorities in descending order.\n"
        "Drag and drop the items in the list to change their priority.");

    if (!right_enabled)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);

    ImGui::BeginChild("priority_list", ImVec2(0, 500), true);

    for (int i = 0; i < ITEM_TYPES_TOTAL; i++)
    {
        item_type_id id = g_config.sorting.item_types_priority[i];
        ImGui::Selectable(item_type_id_to_string(id).c_str(), false);

        if (right_enabled && drag_reorder(g_config.sorting.item_types_priority, i, "TYPE_DRAG"))
            g_config.on_modified();
    }

    ImGui::EndChild();

    if (!right_enabled)
        ImGui::PopStyleVar();

    ImGui::Columns(1);
}

void menu::render_help_button(const char* label, const char* text)
{
    ImGui::SmallButton(label);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", text);
}

bool menu::is_rule_enabled(sort_rule_id id)
{
    for (const auto& [rule_id, enabled] : g_config.sorting.sort_rules_order)
    {
        if (rule_id == id)
            return enabled;
    }
    return true;
}
