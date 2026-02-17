#pragma once
#include <functional>
#include <array>
#include <string>

constexpr wchar_t g_empty_name[] = L"";

constexpr uintptr_t g_priority_table_start_addr = 0x88AFC20;
constexpr uintptr_t g_priority_table_size_addr = 0x88AFC28;

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
	FUNCTIONS_TOTAL
};

inline const char* known_function_id_to_string(known_function_id id)
{
	switch (id)
	{
	case COMPARATOR: return "Comparator";
	case GET_GLOBAL_STATE: return "GetGlobalState";
	case GET_ITEM_BY_DESCRIPTOR: return "GetItemByDescriptor";
	case GET_ITEM_METADATA: return "GetItemMetadata";
	case GET_WEAPON_FROM_ITEM: return "GetWeaponFromItem";
	case GET_SECONDARY_SORT_KEY: return "GetSecondarySortKey";
	case GET_ITEM_NAME: return "GetItemName";
	case COMPARE_ITEM_NAMES: return "CompareItemNames";
	case FREE_ITEM_NAME: return "FreeItemName";
	default: return "Unknown";
	}
}

enum sort_rule_id
{
	SORT_RULE_ID_INVALID = -1,
	COMPARE_QUEST,
	COMPARE_SIZE,
	COMPARE_TYPE_PRIORITY,
	COMPARE_AMMO_CALIBER,
	COMPARE_SECONDARY_KEY,
	COMPARE_NAME,
	RULES_TOTAL
};

inline std::string sort_rule_id_to_readable_string(sort_rule_id id)
{
	switch (id)
	{
	case COMPARE_QUEST:
		return "Prioritize quest items";
	case COMPARE_SIZE:
		return "Sort by size";
	case COMPARE_TYPE_PRIORITY:
		return "Prioritize by type";
	case COMPARE_AMMO_CALIBER:
		return "Sort ammo by caliber";
	case COMPARE_SECONDARY_KEY:
		return "Sort by secondary key";
	case COMPARE_NAME:
		return "Sort by name";
	default:
		return "Unknown";
	}
}

inline std::string sort_rule_id_to_string(sort_rule_id id)
{
	switch (id)
	{
		case COMPARE_QUEST:
			return "COMPARE_QUEST";
		case COMPARE_SIZE:
			return "COMPARE_SIZE";
		case COMPARE_TYPE_PRIORITY:
			return "COMPARE_TYPE_PRIORITY";
		case COMPARE_AMMO_CALIBER:
			return "COMPARE_AMMO_CALIBER";
		case COMPARE_SECONDARY_KEY:
			return "COMPARE_SECONDARY_KEY";
		case COMPARE_NAME:
			return "COMPARE_NAME";
		default:
			return "";
	}
}

inline sort_rule_id sort_rule_id_from_string(const std::string& str)
{
	if (str == "COMPARE_QUEST")
		return COMPARE_QUEST;
	if (str == "COMPARE_SIZE")
		return COMPARE_SIZE;
	if (str == "COMPARE_TYPE_PRIORITY")
		return COMPARE_TYPE_PRIORITY;
	if (str == "COMPARE_AMMO_CALIBER")
		return COMPARE_AMMO_CALIBER;
	if (str == "COMPARE_SECONDARY_KEY")
		return COMPARE_SECONDARY_KEY;
	if (str == "COMPARE_NAME")
		return COMPARE_NAME;
	return SORT_RULE_ID_INVALID;
}

inline std::array<uintptr_t, FUNCTIONS_TOTAL> known_function_offsets = {
	0x59F1528, // comparator
	0x2480D98, // get global state
	0x166FC2C, // get item by descriptor
	0x106EB20, // get item metadata
	0x11BE0E4, // get weapon from item
	0x19F2570, // get secondary sort key
	0x20EE86C, // get item name
	0x3605504, // compare item names
	0x0AEA3D8, // free item name
};

class s2_string;
struct item_info;

using comparator_t = bool(__fastcall*)(void*, void*);
using get_global_state_t = void* (*)();
using get_item_by_descriptor_t = uintptr_t(__fastcall*)(void* root, uint32_t id);
using get_item_metadata_t = uintptr_t(__fastcall*)(uintptr_t);
using get_weapon_from_item_t = uintptr_t(__fastcall*)(uintptr_t);
using get_secondary_sort_key_t = int(__fastcall*)(uintptr_t);
using get_item_name_t = void(__fastcall*)(uintptr_t, s2_string*);
using compare_names_t = int(__fastcall*)(const wchar_t*, const wchar_t*);
using free_s2string_t = void(__fastcall*)(void*);

enum item_type_id: int
{
	ITEM_TYPE_UNKNOWN = -1,
	WEAPON = 0,
	ARMOR,
	ARTIFACT,
	WEAPON_MOD,
	CONSUMABLES,
	AMMO,
	DETECTOR,
	PDA, // PDA and quest-related items
	MISC,
	MUTANT_PARTS,
	NVG,
	ITEM_TYPES_TOTAL
};

inline std::string item_type_id_to_string(item_type_id id)
{
	switch (id)
	{
	case WEAPON:
		return "Weapon";
	case ARMOR:
		return "Armor";
	case ARTIFACT:
		return "Artifact";
	case WEAPON_MOD:
		return "Weapon Mod";
	case CONSUMABLES:
		return "Consumables";
	case AMMO:
		return "Ammo";
	case DETECTOR:
		return "Detector";
	case PDA:
		return "PDA";
	case MISC:
		return "Miscellaneous";
	case MUTANT_PARTS:
		return "Mutant Parts";
	case NVG:
		return "NVG";
	default:
		return "Unknown";
	}
}

inline item_type_id item_type_id_from_string(const std::string& str)
{
	if (str == "Weapon")
		return WEAPON;
	if (str == "Armor")
		return ARMOR;
	if (str == "Artifact")
		return ARTIFACT;
	if (str == "Weapon Mod")
		return WEAPON_MOD;
	if (str == "Consumables")
		return CONSUMABLES;
	if (str == "Ammo")
		return AMMO;
	if (str == "Detector")
		return DETECTOR;
	if (str == "PDA")
		return PDA;
	if (str == "Miscellaneous")
		return MISC;
	if (str == "Mutant Parts")
		return MUTANT_PARTS;
	if (str == "NVG")
		return NVG;
	return ITEM_TYPE_UNKNOWN;
}

class s2_string
{
public:
	s2_string() : m_ptr(nullptr), m_flag(0), m_deleter(nullptr) {}

	s2_string(s2_string&& other) noexcept : m_ptr(other.m_ptr), m_flag(other.m_flag), m_deleter(nullptr)
	{
		other.m_ptr = nullptr;
		other.m_flag = 0;
		other.m_deleter = nullptr;
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

	s2_string(const s2_string&) = delete;
	s2_string& operator=(const s2_string&) = delete;

	~s2_string()
	{
		if (m_ptr && m_deleter)
			m_deleter(m_ptr);
	}

	void set_deleter(free_s2string_t deleter)
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
	wchar_t* m_ptr;
	int m_flag;     // has_string / not_empty / maybe length?
	free_s2string_t m_deleter;
};

struct item_info
{
	s2_string name;
	item_type_id type;
	int secondary_key;
	uint8_t modification;
	uint8_t width;
	uint8_t height;
	bool is_quest;
};
