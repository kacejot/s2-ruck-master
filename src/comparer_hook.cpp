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

    // --- resolve items -----------------------------------------------------
    const auto game_base = global_context::instance().game_base;

    auto get_global_state = reinterpret_cast<void*(*)()>(
        game_base + known_function_offsets[GET_GLOBAL_STATE]);

    auto get_item_by_descriptor = reinterpret_cast<uintptr_t(__fastcall*)(void* root, uint32_t id)>(
        game_base + known_function_offsets[GET_ITEM_BY_DESCRIPTOR]);

    auto get_item_metadata = reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(
        game_base + known_function_offsets[GET_ITEM_METADATA]);

    auto get_weapon_from_item = reinterpret_cast<uintptr_t(__fastcall*)(uintptr_t)>(
        game_base + known_function_offsets[GET_WEAPON_FROM_ITEM]);

    auto get_secondary_sort_key = reinterpret_cast<int(__fastcall*)(uintptr_t)>(
        game_base + known_function_offsets[GET_SECONDARY_SORT_KEY]);

    auto get_item_name = reinterpret_cast<void(__fastcall*)(uintptr_t, void**)>(
        game_base + known_function_offsets[GET_ITEM_NAME]);

    auto compare_names = reinterpret_cast<int(__fastcall*)(void*, void*)>(
        game_base + known_function_offsets[COMPARE_ITEM_NAMES]);

    auto free_item_name = reinterpret_cast<void(__fastcall*)(void*)>(
        game_base + known_function_offsets[FREE_ITEM_NAME]);

    void* global_state = get_global_state();

    void* descriptor_root = *reinterpret_cast<void**>(
        reinterpret_cast<uintptr_t>(global_state) + 4454496
    );

    uintptr_t obj_left = get_item_by_descriptor(descriptor_root, *left & 0x7FFFFFF);
    uintptr_t obj_right = get_item_by_descriptor(descriptor_root, *right & 0x7FFFFFF);

    uintptr_t meta_left = get_item_metadata(obj_left);
    uintptr_t meta_right = get_item_metadata(obj_right);

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


    LOG(Normal, STR("Before compare durability"));
    std::this_thread::sleep_for(1000ms);
    // --- If weapon, compare durability
    if (type_left == 5)
    {
        uintptr_t w1 = get_weapon_from_item(obj_left);
        uintptr_t w2 = get_weapon_from_item(obj_right);

        uint8_t d1 = *(uint8_t*)(w1 + 340);
        uint8_t d2 = *(uint8_t*)(w2 + 340);

        if (d1 != d2)
            return d1 < d2;
    }

    LOG(Normal, STR("Before secondary sort key"));
    std::this_thread::sleep_for(1000ms);
    // --- Secondary Sort Key, TODO: what is this even?
    int k_right = get_secondary_sort_key(obj_right);
    int k_left = get_secondary_sort_key(obj_left);

    if (k_left != k_right)
        return k_left < k_right;

    LOG(Normal, STR("Before compare names"));
    std::this_thread::sleep_for(1000ms);
    // --- Compare Names
    void* name_left = nullptr;
    void* name_right = nullptr;

    uintptr_t metaL = get_item_metadata(obj_left);
    get_item_name(metaL, &name_left);

    uintptr_t metaR = get_item_metadata(obj_right);
    get_item_name(metaR, &name_right);

    bool has_left = (name_left != nullptr);
    bool has_right = (name_right != nullptr);

    void* cmp_left = has_left ? name_left : (void*)L"";
    void* cmp_right = has_right ? name_right : (void*)L"";

    int name_cmp = compare_names(cmp_left, cmp_right);

    bool result = (name_cmp < 0);

    if (has_right) free_item_name(name_right);
    if (has_left)  free_item_name(name_left);

    LOG(Normal, STR("About to return"));
    std::this_thread::sleep_for(1000ms);
    return result;
}
