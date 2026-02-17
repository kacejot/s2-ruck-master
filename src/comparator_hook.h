#pragma once
#include <cstdint>
#include <string>

void init_comparator(uintptr_t game_base);
void deinit_comparator();
bool reinit_comparator();

std::string get_comparator_error();

bool __fastcall comparator_hook(uint32_t* left_ptr, uint32_t* right_ptr);
