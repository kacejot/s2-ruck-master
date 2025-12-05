#include "comparator_hook.h"
#include "context.h"

namespace
{
    inline bool descriptor_is_valid(uint32_t descriptor)
    {
        return descriptor < 0xFFFFFFF0;
    }
}

comparator_hook::comparator_hook(context& ctx) : 
    m_ctx{ ctx }, 
    m_sort_rules
    {
        { COMPARE_QUEST,           &comparator_hook::compare_quest_items },
        { COMPARE_SIZE,            &comparator_hook::compare_size },
        { COMPARE_TYPE_PRIORITY,   &comparator_hook::compare_types_in_priority_table },
        { COMPARE_AMMO_CALIBER,    &comparator_hook::compare_ammo },
        { COMPARE_SECONDARY_KEY,   &comparator_hook::compare_secondary_keys },
        { COMPARE_NAME,            &comparator_hook::compare_names },
    }
{
}

bool comparator_hook::operator()(comparator_t, uint32_t* left_ptr, uint32_t* right_ptr)
{
    auto left_descriptor = *left_ptr;
    auto right_descriptor = *right_ptr;

    auto left_is_valid = descriptor_is_valid(left_descriptor);
    auto right_is_valid = descriptor_is_valid(right_descriptor);
    if (!left_is_valid || !right_is_valid)
        return right_is_valid;

    auto left_item = load_item_by_descriptor(left_descriptor);
    auto right_item = load_item_by_descriptor(right_descriptor);

    for (const auto& [rule_id, rule_enabled] : m_ctx.config.sort_rules_order)
    {
        if (!rule_enabled)
 	        continue;

    const auto& rule = m_sort_rules[rule_id];
        auto result = (this->*rule)(left_item, right_item);
        if (std::strong_ordering::equal == result)
            continue;

        return std::strong_ordering::less == result;
    }

    return true;
}

item_info comparator_hook::load_item_by_descriptor(uint32_t item)
{
    item_info result;

    const auto game_base = m_ctx.game_base;
    auto& funcs = m_ctx.functions;

    void* global_object_pool = funcs.get_global_object_pool();
    void* descriptor_root = (void*)((uintptr_t)global_object_pool + 0x43F860);
    uintptr_t obj = funcs.get_item_by_descriptor(descriptor_root, item & 0x7FFFFFF);
    uintptr_t meta = funcs.get_item_metadata(obj);

    funcs.get_item_name(meta, &result.name);
    result.name.set_deleter(funcs.free_s2string);

    result.width = *reinterpret_cast<uint8_t*>(meta + 202);
    result.height = *reinterpret_cast<uint8_t*>(meta + 203);
    result.type = *reinterpret_cast<item_type_id*>(meta + 208);
    result.is_quest = (*(uint8_t*)(obj + 65) & 2) != 0;
    result.secondary_key = funcs.get_secondary_sort_key(obj);
    if (AMMO == result.type)
    {
        result.modification = *(uint8_t*)(funcs.get_weapon_from_item(obj) + 340);
    }

    return result;
}

std::strong_ordering comparator_hook::compare_quest_items(const item_info& left, const item_info& right)
{
    if (left.is_quest && !right.is_quest)
        return std::strong_ordering::less;
    if (!left.is_quest && right.is_quest)
        return std::strong_ordering::greater;
    return std::strong_ordering::equal;
}

std::strong_ordering comparator_hook::compare_size(const item_info& left, const item_info& right)
{
    auto width_cmp = right.width <=> left.width;
    if (std::strong_ordering::equal != width_cmp)
        return width_cmp;

    return right.height <=> left.height;
}

std::strong_ordering comparator_hook::compare_types_in_priority_table(const item_info& left, const item_info& right)
{
    const auto game_base = m_ctx.game_base;

    if (left.type != right.type)
    {
        const auto& table = m_ctx.config.item_types_priority;


        int idx_left = -1;
        for (const auto priority : table)
        {
            idx_left++;
            if (priority == left.type)
            {
                break;
            }
        }

        int idx_right = -1;
        for (const auto priority : table)
        {
            idx_right++;
            if (priority == right.type)
            {
                break;
            }
        }

        return idx_left <=> idx_right;
    }

    return std::strong_ordering::equal;
}

std::strong_ordering comparator_hook::compare_ammo(const item_info& left, const item_info& right)
{
    if (AMMO == left.type && AMMO == right.type)
        return left.modification <=> right.modification; // in context of ammo, modification is weapon type
    return std::strong_ordering::equal;
}

std::strong_ordering comparator_hook::compare_secondary_keys(const item_info& left, const item_info& right)
{
    return left.secondary_key <=> right.secondary_key; // in context of weapons, secondary key is durability
}

std::strong_ordering comparator_hook::compare_names(const item_info& left, const item_info& right)
{
    const wchar_t* cmp_left_ptr = !left.name.empty() ? left.name.c_str() : g_empty_name;
    const wchar_t* cmp_right_ptr = !right.name.empty() ? right.name.c_str() : g_empty_name;
    return m_ctx.functions.compare_names(cmp_left_ptr, cmp_right_ptr) <=> 0;
}
