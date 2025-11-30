#pragma once
#include <functional>
#include <array>

#include "type_traits.h"

// constants

constexpr wchar_t g_empty_name[] = L"";

constexpr uintptr_t g_priority_table_start_addr = 0x88AFC20;
constexpr uintptr_t g_priority_table_size_addr = 0x88AFC28;

// function info

enum known_function_id
{
	COMPARATOR = 0,
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
	0x1240FC0, // comparator
	0x18148C4, // get global state
	0x143F2D0, // get item by descriptor
	0x0AF4818, // get item metadata
	0x11BFBF8, // get weapon from item
	0x10BA52C, // get secondary sort key
	0x1241208, // get item name
	0x293F9A4, // compare item names
	0x0B530D4, // free item name
};

class s2_string;

using comparator_t = bool(__fastcall*)(void*, void*);
using get_global_state_t = void* (*)();
using get_item_by_descriptor_t = uintptr_t(__fastcall*)(void* root, uint32_t id);
using get_item_metadata_t = uintptr_t(__fastcall*)(uintptr_t);
using get_weapon_from_item_t = uintptr_t(__fastcall*)(uintptr_t);
using get_secondary_sort_key_t = int(__fastcall*)(uintptr_t);
using get_item_name_t = void(__fastcall*)(uintptr_t, s2_string*);
using compare_names_t = int(__fastcall*)(const wchar_t*, const wchar_t*);
using free_s2string_t = void(__fastcall*)(void*);

struct known_functions
{
	function_signature_t<get_global_state_t>          get_global_object_pool;
	function_signature_t<get_item_by_descriptor_t>    get_item_by_descriptor;
	function_signature_t<get_item_metadata_t>         get_item_metadata;
	function_signature_t<get_weapon_from_item_t>      get_weapon_from_item;
	function_signature_t<get_secondary_sort_key_t>    get_secondary_sort_key;
	function_signature_t<get_item_name_t>             get_item_name;
	function_signature_t<compare_names_t>             compare_names;
	function_signature_t<free_s2string_t>             free_s2string;
};

enum item_type
{
	ITEM_TYPE_UNKNOWN = -1,
	WEAPON = 0,
	ARMOR,
	ARTIFACT,
	WEAPON_MOD,
	CONSUMABLES,
	AMMO,
	DETECTOR,
	UNDEFINED_1, // TODO: Maybe add log to track this specific type usage
	MISC,
	MUTANT_PARTS,
	NVG,
};

class s2_string
{
public:
	s2_string() : m_deleter(nullptr), m_ptr(nullptr), m_flag(0) {}

	// move
	s2_string(s2_string&& other) noexcept : m_ptr(other.m_ptr), m_flag(other.m_flag)
	{
		other.m_ptr = nullptr;
		other.m_flag = 0;
	}

	s2_string& operator=(s2_string&& other) noexcept
	{
		if (this != &other)
		{
			if (m_ptr && m_deleter)
				m_deleter(m_ptr);

			m_ptr = other.m_ptr;
			m_flag = other.m_flag;
			other.m_ptr = nullptr;
			other.m_flag = 0;
		}
		return *this;
	}

	// forbid copy
	s2_string(const s2_string&) = delete;
	s2_string& operator=(const s2_string&) = delete;

	~s2_string()
	{
		if (m_ptr && m_deleter)
			m_deleter(m_ptr);
	}

	void set_deleter(function_signature_t<free_s2string_t> deleter)
	{
		m_deleter = deleter;
	}

	bool empty() const
	{
		return m_ptr == nullptr || m_flag == 0;
	}

	const wchar_t* c_str() const
	{
		return m_ptr ? m_ptr : L"";
	}

private:
	wchar_t* m_ptr; // UTF-16 char buffer
	int m_flag;     // has_string / not_empty / maybe length?
	function_signature_t<free_s2string_t> m_deleter;
};

struct item_info
{
	s2_string name;
	int type;
	int secondary_key; // TODO: wtf is this
	uint8_t modification;
	uint8_t width;
	uint8_t height;
	bool is_quest;
};
