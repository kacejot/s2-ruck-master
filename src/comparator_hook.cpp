#include "comparator_hook.h"

#include <thread>
#include <chrono>
#include <compare>

#include "global_context.h"
#include "print.h"

using namespace std::chrono_literals;

namespace
{
    bool descriptor_is_valid(uint32_t descriptor)
    {
        return descriptor < 0xFFFFFFF0;
	}

    item_info load_item_by_descriptor(uint32_t item)
    {
        item_info result;

        const auto game_base = get_ctx().game_base;
        auto& funcs = get_functions();

        void* global_object_pool = funcs.get_global_object_pool();
        void* descriptor_root = (void*)((uintptr_t)global_object_pool + 0x43F860);
        uintptr_t obj = funcs.get_item_by_descriptor(descriptor_root, item & 0x7FFFFFF);
        uintptr_t meta = funcs.get_item_metadata(obj);

        funcs.get_item_name(meta, &result.name);
		result.name.set_deleter(funcs.free_s2string);

        result.width = *reinterpret_cast<uint8_t*>(meta + 202);
        result.height = *reinterpret_cast<uint8_t*>(meta + 203);
        result.type = *reinterpret_cast<int*>(meta + 208);
        result.is_quest = (*(uint8_t*)(obj + 65) & 2) != 0;
        result.secondary_key = funcs.get_secondary_sort_key(obj);
        if (AMMO == result.type)
        {
            result.modification = *(uint8_t*)(funcs.get_weapon_from_item(obj) + 340);
        }

#ifdef UE_BUILD_DEVELOPMENT
        LOG(Verbose, std::format(
            STR(" --- item with name {} has type {} | width: {}, height: {}, is_quest: {}, durability: {}"),
            result.name.c_str(),
            result.type,
            result.width,
            result.height,
            result.is_quest ? STR("true") : STR("false"),
            result.modification
        ));
#endif
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
		auto width_cmp = right.width <=> left.width  ;
        if (std::strong_ordering::equal != width_cmp)
            return width_cmp;

		return right.height <=> left.height;
    }

    std::strong_ordering compare_types_in_priority_table(const item_info& left, const item_info& right)
    {
        const auto game_base = get_ctx().game_base;

        if (left.type != right.type)
        {
            uint32_t* table = *reinterpret_cast<uint32_t**>(game_base + g_priority_table_start_addr);
            int count = *reinterpret_cast<int*>(game_base + g_priority_table_size_addr);

            uint32_t* begin = table;
            uint32_t* end = table + count;

            int idx_left = -1;
            for (uint32_t* p = begin; p != end; ++p)
            {
                if (*p == left.type)
                {
                    idx_left = int(p - begin);
                    break;
                }
            }

            int idx_right = -1;
            for (uint32_t* p = begin; p != end; ++p)
            {
                if (*p == right.type)
                {
                    idx_right = int(p - begin);
                    break;
                }
            }

            return idx_left <=> idx_right;
        }

		return std::strong_ordering::equal;
    }

    std::strong_ordering compare_ammo(const item_info& left, const item_info& right)
    {
        if (AMMO == left.type && AMMO == right.type)
        {
			return left.modification <=> right.modification; // in context of ammo, modification is weapon type
        }
		return std::strong_ordering::equal;
    }

    std::strong_ordering compare_secondary_keys(const item_info& left, const item_info& right)
    {
		return left.secondary_key <=> right.secondary_key; // in context of weapons, secondary key is durability
    }

    std::strong_ordering compare_names(const item_info& left, const item_info& right)
    {
        const wchar_t* cmp_left_ptr = !left.name.empty() ? left.name.c_str() : g_empty_name;
        const wchar_t* cmp_right_ptr = !right.name.empty() ? right.name.c_str() : g_empty_name;
        return get_functions().compare_names(cmp_left_ptr, cmp_right_ptr) <=> 0;
	}

	using attribute_comparator_t = std::function<std::strong_ordering(const item_info&, const item_info&)>;
    static std::vector<attribute_comparator_t> g_sort_rules =
    {
        compare_quest_items,
        compare_size,
        compare_types_in_priority_table,
        compare_ammo,
        compare_secondary_keys,
        compare_names,
    };
}

bool __fastcall comparator(comparator_t, uint32_t* left_ptr, uint32_t* right_ptr)
{
	auto left_descriptor = *left_ptr;
	auto right_descriptor = *right_ptr;

	auto left_is_valid = descriptor_is_valid(left_descriptor);
    auto right_is_valid = descriptor_is_valid(right_descriptor);
    if (!left_is_valid || !right_is_valid)
        return right_is_valid;

	auto& funcs = get_functions();
    
    auto left_item = load_item_by_descriptor(left_descriptor);
    auto right_item = load_item_by_descriptor(right_descriptor);

    for (auto&& rule : g_sort_rules)
    {
        auto result = rule(left_item, right_item);
        if (std::strong_ordering::equal == result)
            continue;

        return std::strong_ordering::less == result;
    }

    return true;
}
