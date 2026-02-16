#include "comparator_hook.h"
#include "config.h"

#include <windows.h>
#include <map>

namespace
{
    // Game function pointers (initialized by init_comparator)
    get_global_state_t          g_get_global_object_pool = nullptr;
    get_item_by_descriptor_t    g_get_item_by_descriptor = nullptr;
    get_item_metadata_t         g_get_item_metadata = nullptr;
    get_weapon_from_item_t      g_get_weapon_from_item = nullptr;
    get_secondary_sort_key_t    g_get_secondary_sort_key = nullptr;
    get_item_name_t             g_get_item_name = nullptr;
    compare_names_t             g_compare_names = nullptr;
    free_s2string_t             g_free_s2string = nullptr;

    inline bool descriptor_is_valid(uint32_t descriptor)
    {
        return descriptor < 0xFFFFFFF0;
    }

    item_info load_item_by_descriptor(uint32_t item)
    {
        item_info result;

        void* global_object_pool = g_get_global_object_pool();
        void* descriptor_root = (void*)((uintptr_t)global_object_pool + 0x43F860);
        uintptr_t obj = g_get_item_by_descriptor(descriptor_root, item & 0x7FFFFFF);
        uintptr_t meta = g_get_item_metadata(obj);

        g_get_item_name(meta, &result.name);
        result.name.set_deleter(g_free_s2string);

        result.width = *reinterpret_cast<uint8_t*>(meta + 202);
        result.height = *reinterpret_cast<uint8_t*>(meta + 203);
        result.type = *reinterpret_cast<item_type_id*>(meta + 208);
        result.is_quest = (*(uint8_t*)(obj + 65) & 2) != 0;
        result.secondary_key = g_get_secondary_sort_key(obj);
        if (AMMO == result.type)
        {
            result.modification = *(uint8_t*)(g_get_weapon_from_item(obj) + 340);
        }

        return result;
    }

    std::strong_ordering compare_quest_items(const item_info& left, const item_info& right)
    {
        if (left.is_quest && !right.is_quest)
            return std::strong_ordering::less;
        if (!left.is_quest && right.is_quest)
            return std::strong_ordering::greater;
        return std::strong_ordering::equal;
    }

    std::strong_ordering compare_size(const item_info& left, const item_info& right)
    {
        auto width_cmp = right.width <=> left.width;
        if (std::strong_ordering::equal != width_cmp)
            return width_cmp;

        return right.height <=> left.height;
    }

    std::strong_ordering compare_types_in_priority_table(const item_info& left, const item_info& right)
    {
        if (left.type != right.type)
        {
            const auto& table = g_config.sorting.item_types_priority;

            int idx_left = -1;
            for (const auto priority : table)
            {
                idx_left++;
                if (priority == left.type)
                    break;
            }

            int idx_right = -1;
            for (const auto priority : table)
            {
                idx_right++;
                if (priority == right.type)
                    break;
            }

            return idx_left <=> idx_right;
        }

        return std::strong_ordering::equal;
    }

    std::strong_ordering compare_ammo(const item_info& left, const item_info& right)
    {
        if (AMMO == left.type && AMMO == right.type)
            return left.modification <=> right.modification;
        return std::strong_ordering::equal;
    }

    std::strong_ordering compare_secondary_keys(const item_info& left, const item_info& right)
    {
        return left.secondary_key <=> right.secondary_key;
    }

    std::strong_ordering compare_names(const item_info& left, const item_info& right)
    {
        const wchar_t* cmp_left_ptr = !left.name.empty() ? left.name.c_str() : g_empty_name;
        const wchar_t* cmp_right_ptr = !right.name.empty() ? right.name.c_str() : g_empty_name;
        return g_compare_names(cmp_left_ptr, cmp_right_ptr) <=> 0;
    }

    using attribute_comparator_t = std::strong_ordering(*)(const item_info&, const item_info&);
    std::map<sort_rule_id, attribute_comparator_t> g_sort_rules = {
        { COMPARE_QUEST,           compare_quest_items },
        { COMPARE_SIZE,            compare_size },
        { COMPARE_TYPE_PRIORITY,   compare_types_in_priority_table },
        { COMPARE_AMMO_CALIBER,    compare_ammo },
        { COMPARE_SECONDARY_KEY,   compare_secondary_keys },
        { COMPARE_NAME,            compare_names },
    };
}

void init_comparator(uintptr_t game_base)
{
    g_get_global_object_pool = get_global_state_t(game_base + known_function_offsets[GET_GLOBAL_STATE]);
    g_get_item_by_descriptor = get_item_by_descriptor_t(game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]);
    g_get_item_metadata = get_item_metadata_t(game_base + known_function_offsets[GET_ITEM_METADATA]);
    g_get_weapon_from_item = get_weapon_from_item_t(game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]);
    g_get_secondary_sort_key = get_secondary_sort_key_t(game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]);
    g_get_item_name = get_item_name_t(game_base + known_function_offsets[GET_ITEM_NAME]);
    g_compare_names = compare_names_t(game_base + known_function_offsets[COMPARE_ITEM_NAMES]);
    g_free_s2string = free_s2string_t(game_base + known_function_offsets[FREE_ITEM_NAME]);
}

bool __fastcall comparator_hook(uint32_t* left_ptr, uint32_t* right_ptr)
{
    auto left_descriptor = *left_ptr;
    auto right_descriptor = *right_ptr;

    auto left_is_valid = descriptor_is_valid(left_descriptor);
    auto right_is_valid = descriptor_is_valid(right_descriptor);
    if (!left_is_valid || !right_is_valid)
        return right_is_valid;

    auto left_item = load_item_by_descriptor(left_descriptor);
    auto right_item = load_item_by_descriptor(right_descriptor);

    for (const auto& [rule_id, rule_enabled] : g_config.sorting.sort_rules_order)
    {
        if (!rule_enabled)
            continue;

        const auto& rule = g_sort_rules[rule_id];
        auto result = rule(left_item, right_item);
        if (std::strong_ordering::equal == result)
            continue;

        return std::strong_ordering::less == result;
    }

    return true;
}
