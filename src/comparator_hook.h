#pragma once 
#include <map>
#include "local_types.h"

struct context;

class comparator_hook
{
public:
	comparator_hook(context& ctx);
	bool operator()(comparator_t, uint32_t* left_ptr, uint32_t* right_ptr);

private:
    item_info load_item_by_descriptor(uint32_t item);

    std::strong_ordering compare_quest_items(const item_info& left, const item_info& right);
    std::strong_ordering compare_size(const item_info& left, const item_info& right);
    std::strong_ordering compare_types_in_priority_table(const item_info& left, const item_info& right);
    std::strong_ordering compare_ammo(const item_info& left, const item_info& right);
    std::strong_ordering compare_secondary_keys(const item_info& left, const item_info& right);
    std::strong_ordering compare_names(const item_info& left, const item_info& right);

private:
	context& m_ctx;
    using attribute_comparator_t = std::strong_ordering(comparator_hook::*)(const item_info&, const item_info&);
	std::map<sort_rule_id, attribute_comparator_t> m_sort_rules;
};
