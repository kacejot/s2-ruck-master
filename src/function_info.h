#pragma once
#include <functional>
#include <array>

#include "lambda_traits.h"

constexpr wchar_t g_empty_name[] = L"";

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

struct S2String {
	void* ptr;   // UTF-16 char buffer (wchar_t*)
	int flag;    // has_string / not_empty
};

using comparer_original_t = bool(__fastcall*)(void*, void*);
using get_global_state_t = void* (*)();
using get_item_by_descriptor_t = uintptr_t(__fastcall*)(void* root, uint32_t id);
using get_item_metadata_t = uintptr_t(__fastcall*)(uintptr_t);
using get_weapon_from_item_t = uintptr_t(__fastcall*)(uintptr_t);
using get_secondary_sort_key_t = int(__fastcall*)(uintptr_t);
using get_item_name_t = void(__fastcall*)(uintptr_t, S2String*);
using compare_names_t = int(__fastcall*)(void*, void*);
using free_item_name_t = void(__fastcall*)(void*);

struct known_functions
{
	function_signature_t<get_global_state_t>          get_global_state;
	function_signature_t<get_item_by_descriptor_t>    get_item_by_descriptor;
	function_signature_t<get_item_metadata_t>         get_item_metadata;
	function_signature_t<get_weapon_from_item_t>      get_weapon_from_item;
	function_signature_t<get_secondary_sort_key_t>    get_secondary_sort_key;
	function_signature_t<get_item_name_t>             get_item_name;
	function_signature_t<compare_names_t>             compare_names;
	function_signature_t<free_item_name_t>            free_item_name;
};
