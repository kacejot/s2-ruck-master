#pragma once
#include <cstdint>

void init_comparator(uintptr_t game_base);
bool __fastcall comparator_hook(uint32_t* left_ptr, uint32_t* right_ptr);
