#pragma once
#include <array>

constexpr uintptr_t g_priority_table_start_addr = 0x88AFC20;
constexpr uintptr_t g_priority_table_size_addr = 0x88AFC28;

enum known_function_id
{
	COMPARER = 0,
	GET_GLOBAL_STATE,
	GET_ITEM_BY_DESCRIPTOR,
	GET_ITEM_METADATA,
	GET_WEAPON_FROM_ITEM,
	GET_SECONDARY_SORT_KEY,
	GET_ITEM_NAME,
	COMPARE_ITEM_NAMES,
	FREE_ITEM_NAME,
	TOTAL_FUNCTIONS
};

static constexpr std::array<uintptr_t, TOTAL_FUNCTIONS> known_function_offsets = {
	0x1240FC0, // comparer
	0x18148C4, // get global state
	0x143F2D0, // get item by descriptor
	0x0AF4818, // get item metadata
	0x11BFBF8, // get weapon from item
	0x10BA52C, // get secondary sort key
	0x1241208, // get item name
	0x293F9A4, // compare item names
	0x0B530D4, // free item name
};

using comparer_original_t = bool(__fastcall*)(void*, void*);
