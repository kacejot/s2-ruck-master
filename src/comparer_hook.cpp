#include "comparer_hook.h"

#include <thread>
#include <chrono>

#include "global_context.h"
#include "print.h"

using namespace std::chrono_literals;

bool __fastcall Comparer(comparer_original_t, void* left_ptr, void* right_ptr)
{
    auto left = reinterpret_cast<uint32_t*>(left_ptr);
    auto right = reinterpret_cast<uint32_t*>(right_ptr);

    // 1. Check for invalid handles (0xFFFFFFF0-style)
    if (((*left & 0xFFFFFFF0) == 0xFFFFFFF0) ||
        ((*right & 0xFFFFFFF0) == 0xFFFFFFF0))
        return ((*right & 0xFFFFFFF0) != 0xFFFFFFF0);

    const auto game_base = get_ctx().game_base;
	auto& funcs = get_ctx().wrappers;

    void* global_state_left = funcs.get_global_state();
    void* descriptor_root_left = (void*)((uintptr_t)global_state_left + 0x43F860);
    uintptr_t obj_left = funcs.get_item_by_descriptor(descriptor_root_left, *left & 0x7FFFFFF);
    uintptr_t meta_left = funcs.get_item_metadata(obj_left);
    S2String name_left{};
    funcs.get_item_name(meta_left, &name_left);

	void* global_state_right = funcs.get_global_state();
	void* descriptor_root_right = (void*)((uintptr_t)global_state_right + 0x43F860);
    uintptr_t obj_right = funcs.get_item_by_descriptor(descriptor_root_right, *right & 0x7FFFFFF);
    uintptr_t meta_right = funcs.get_item_metadata(obj_right);
    S2String name_right{};
    funcs.get_item_name(meta_right, &name_right);

    LOG(Verbose, std::format(STR(" --- comparing items: \"{}\" vs \"{}\""), name_left.flag ? (wchar_t*)name_left.ptr : STR("<none>"), name_right.flag ? (wchar_t*)name_right.ptr : STR("<none>")));

    int type_left = *reinterpret_cast<int*>(meta_left + 208);
    int type_right = *reinterpret_cast<int*>(meta_right + 208);

    // --- Quest Items Always First
    bool is_quest_left = (*(uint8_t*)(obj_left + 65) & 2) != 0;
    bool is_quest_right = (*(uint8_t*)(obj_right + 65) & 2) != 0;

    if (is_quest_left && !is_quest_right)
        return true;
    if (is_quest_right && !is_quest_left)
        return false;

    // --- Priority Table
    if (type_left != type_right)
    {
        uint32_t* table = reinterpret_cast<uint32_t*>(game_base + g_priority_table_start_addr);
        int count = *reinterpret_cast<int*>(game_base + g_priority_table_size_addr);

        uint32_t* begin = table;
        uint32_t* end = table + count;

        int idx_left = -1;
        for (uint32_t* p = begin; p != end; ++p)
        {
            if (*p == type_left)
            {
                idx_left = int(p - begin);
                break;
            }
        }

        int idx_right = -1;
        for (uint32_t* p = begin; p != end; ++p)
        {
            if (*p == type_right)
            {
                idx_right = int(p - begin);
                break;
            }
        }

        return idx_left < idx_right;
    }

    // --- If weapon, compare durability
    if (type_left == 5)
    {
        uintptr_t w1 = funcs.get_weapon_from_item(obj_left);
        uintptr_t w2 = funcs.get_weapon_from_item(obj_right);

        uint8_t d1 = *(uint8_t*)(w1 + 340);
        uint8_t d2 = *(uint8_t*)(w2 + 340);

        if (d1 != d2)
            return d1 < d2;
    }

    // --- Secondary Sort Key, TODO: what is this even?
    int k_right = funcs.get_secondary_sort_key(obj_right);
    int k_left = funcs.get_secondary_sort_key(obj_left);

    if (k_left != k_right)
        return k_left < k_right;

    // --- Compare Names
    void* cmp_left_ptr = name_left.ptr ? name_left.ptr : (void*)g_empty_name;
    void* cmp_right_ptr = name_right.ptr ? name_right.ptr : (void*)g_empty_name;

    int name_cmp = funcs.compare_names(cmp_left_ptr, cmp_right_ptr);
    bool result = (name_cmp < 0);

    LOG(Verbose, std::format(STR("--- {}'s name is bigger"), result ? STR("right") : STR("left")));

    if (name_left.ptr)
        funcs.free_item_name(name_left.ptr);

    if (name_right.ptr)
        funcs.free_item_name(name_right.ptr);

    return result;
}
