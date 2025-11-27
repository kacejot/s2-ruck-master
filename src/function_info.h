#pragma once

enum known_function_id
{
	COMPARER,
	TOTAL_FUNCTIONS,
};

static constexpr std::array<uintptr_t, TOTAL_FUNCTIONS> known_function_offsets = {
	0x1240FC0, // sort comparer
};

using comparer_original_t = bool(__fastcall*)(void*, void*);
