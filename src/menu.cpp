#include "menu.h"
#include "global_context.h"

bool IsRuleEnabled(sort_rule_id id)
{
    for (auto& r : get_ctx().config.sort_rules_order)
        if (r.first == id)
            return r.second;
    return true;
}

void RenderPresets()
{
    ImGui::Text("Presets:");

    ImGui::SetNextItemWidth(300);
	auto current_preset = get_ctx().current_config_preset;
    std::string preset_name_str = preset_id_to_string(current_preset);
    const char* preset_name = preset_name_str.c_str();

    if (ImGui::BeginCombo("##preset", preset_name))
    {
        for (int i = 0; i < PRESETS_TOTAL; i++)
        {
			preset_id p_id = (preset_id)i;
            bool sel = (current_preset == p_id);
            if (ImGui::Selectable(preset_id_to_string(p_id).c_str(), sel))
            {
                get_ctx().switch_preset(p_id);
            }

            if (sel)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
}

void RenderLists()
{
    bool right_enabled = IsRuleEnabled(COMPARE_TYPE_PRIORITY);

    ImGui::Columns(2);
    ImGui::Text("Rules");
    ImGui::SameLine();
    RenderHelpButton("?##rules_small_button", "Rule at the top of the list has the highest priority.\nIf rule considers two items equal, next rule in the list will process them.\nDrag and drop the rules to change their priority.\nClick on the rules to toggle them.\n\nNOTE: I'm not sure what \"Sort by secondary key\" is. I'm most certain that it is a durability for weapons.");

    ImGui::BeginChild("rules_list", ImVec2(0, 500), true);

    for (int i = 0; i < RULES_TOTAL; i++)
    {
		auto& ctx = get_ctx();
        auto& [rule_id, rule_enabled] = ctx.config.sort_rules_order[i];

        if (!rule_enabled)
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.5, 0.5, 0.5, 1));

        bool clicked = ImGui::Selectable(sort_rule_id_to_readable_string(rule_id).c_str(), false);

        if (!rule_enabled)
            ImGui::PopStyleColor();

        if (DragReorder(ctx.config.sort_rules_order, i, "RULE_DRAG"))
			ctx.switch_preset(CUSTOM);

        if (clicked)
        {
            rule_enabled = !rule_enabled;
			ctx.switch_preset(CUSTOM);
        }
    }

    ImGui::EndChild();
    ImGui::NextColumn();
    ImGui::Text("Item type priorities");
    ImGui::SameLine();
    RenderHelpButton("?##type_priorities_small_button", "NOTE: This list works only if \"Prioritize by type\" rule is active.\n\nItem type priorities in descending order.\nDrag and drop the items in the list to change their priority.");

    if (!right_enabled)
        ImGui::PushStyleVar(ImGuiStyleVar_Alpha, 0.3f);

    ImGui::BeginChild("priority_list", ImVec2(0, 500), true);

    for (int i = 0; i < ITEM_TYPES_TOTAL; i++)
    {
        item_type_id id = get_ctx().config.item_types_priority[i];
        ImGui::Selectable(item_type_id_to_string(id).c_str(), false);

        if (right_enabled && DragReorder(get_ctx().config.item_types_priority, i, "TYPE_DRAG"))
			get_ctx().switch_preset(CUSTOM);
    }

    ImGui::EndChild();

    if (!right_enabled)
        ImGui::PopStyleVar();

    ImGui::Columns(1);
}

void RenderHelpButton(const char* label, const char* text)
{
    ImGui::SmallButton(label);
    if (ImGui::IsItemHovered())
        ImGui::SetTooltip("%s", text);
}

void RenderFlags()
{
    auto& ctx = get_ctx();
    bool before = ctx.config.enable_logging;
    if (ImGui::Checkbox("enable logs", &ctx.config.enable_logging))
        if (ctx.config.enable_logging != before)
            ctx.switch_preset(CUSTOM);
}
