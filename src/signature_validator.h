#pragma once
#include <cstdint>
#include <string>
#include <bitset>
#include <array>
#include <optional>
#include <atomic>
#include "local_types.h"

enum class ValidationResult
{
    SUCCESS,
    INVALID_ADDRESS,
    SIGNATURE_MISMATCH
};

struct Signature
{
    static constexpr size_t MAX_LENGTH = 128;
    
    uint8_t bytes[MAX_LENGTH];
    std::bitset<MAX_LENGTH> mask;
    size_t length;
    
    Signature(const char* pattern);
    
    ValidationResult validate(uintptr_t address) const;
    std::optional<uintptr_t> scan(uintptr_t start, size_t size) const;
};

inline constexpr std::array<const char*, FUNCTIONS_TOTAL> known_function_signatures = {
    "48 89 5C 24 ? 48 89 74 24 ? 55 57 41 56 48 8B EC 48 83 EC ? 8B 01",
    "48 83 EC ? 65 48 8B 04 25 ? ? ? ? BA ? ? ? ? 48 8B 08 8B 04 0A 39 05 ? ? ? ? 7F ? 48 8D 05 ? ? ? ? 48 83 C4 ? C3 48 8D 0D ? ? ? ? E8 ? ? ? ? 83 3D ? ? ? ? ? 75 ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? 48 8D 0D ? ? ? ? E8 ? ? ? ? EB ? 48 89 4C 24",
    "81 FA ? ? ? ? 72 ? 48 8B 09 81 C2 ? ? ? ? E9 ? ? ? ? 48 63 C2 48 69 D0 ? ? ? ? 48 8D 41 ? 48 03 C2 C3 40 53 48 83 EC ? 48 8B 19",
    "40 53 48 83 EC ? 48 83 C1 ? 48 8D 44 24 ? 8B 19 89 5C 24 ? 48 3B C8 74 ? 85 DB 74 ? 8B CB C1 E9 ? 84 C9 75 ? 48 8D 0D ? ? ? ? FF 15 ? ? ? ? 8B C3 48 8D 0D ? ? ? ? 99 41 B8 ? ? ? ? 41 F7 F8 44 8B C2 48 8D 15 ? ? ? ? 48 98 49 C1 E0 ? 48 8B 04 C2 42 FF 44 00 ? FF 15 ? ? ? ? 48 8D 4C 24 ? E8 ? ? ? ? 48 83 C4 ? 5B C3 CC CC 48 89 5C 24 ? 57",
    "48 83 EC ? 48 8D 51 ? 48 8D 4C 24 ? E8 ? ? ? ? 48 8B C8 E8 ? ? ? ? 48 83 C4 ? C3 CC 48 89 5C 24 ? 48 89 4C 24",
    "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC ? 48 8B F9 0F 29 74 24 ? 48 8B 49 ? 48 85 C9",
    "48 89 5C 24 ? 57 48 83 EC ? 48 8B DA 48 8D 54 24 ? E8 ? ? ? ? E8",
    "48 89 5C 24 ? 4C 8B D9 48 8D 1D ? ? ? ? 4C 2B DA 45 33 C0 45 0F B7 0C 13",
    "48 85 C9 0F 84 ? ? ? ? 55 53 56 57"
};

std::string validate_all_addresses(uintptr_t game_base);

struct ScanResult
{
    bool success = false;
    std::array<uintptr_t, FUNCTIONS_TOTAL> offsets = {};
    int found_count = 0;
};

ScanResult scan_all_signatures(uintptr_t game_base, std::atomic<float>& progress);

