#include "signature_validator.h"
#include "local_types.h"
#include <cstring>
#include <sstream>
#include <windows.h>

Signature::Signature(const char* pattern)
{
    length = 0;
    mask.reset();
    
    const char* p = pattern;
    while (*p && length < MAX_LENGTH)
    {
        while (*p == ' ') p++;
        if (!*p) break;
        
        if (*p == '?')
        {
            bytes[length] = 0;
            length++;
            p++;
        }
        else
        {
            char hex[] = { p[0], p[1], '\0' };
            bytes[length] = (uint8_t)strtoul(hex, nullptr, 16);
            mask[length] = true;
            length++;
            p += 2;
        }
    }
}

ValidationResult Signature::validate(uintptr_t address) const
{
    if (address == 0)
        return ValidationResult::INVALID_ADDRESS;
    
    const uint8_t* memory = (const uint8_t*)address;
    
    for (size_t i = 0; i < length; i++)
    {
        if (mask[i])
        {
            if (memory[i] != bytes[i])
                return ValidationResult::SIGNATURE_MISMATCH;
        }
    }
    
    return ValidationResult::SUCCESS;
}

std::optional<uintptr_t> Signature::scan(uintptr_t start, size_t size) const
{
    if (size < length)
        return std::nullopt;
    
    const uint8_t* memory = (const uint8_t*)start;
    const size_t end = size - length;
    
    for (size_t i = 0; i <= end; i++)
    {
        bool match = true;
        for (size_t j = 0; j < length; j++)
        {
            if (mask[j] && memory[i + j] != bytes[j])
            {
                match = false;
                break;
            }
        }
        
        if (match)
            return start + i;
    }
    
    return std::nullopt;
}

std::string validate_all_addresses(uintptr_t game_base)
{
    PIMAGE_DOS_HEADER dos = reinterpret_cast<PIMAGE_DOS_HEADER>(game_base);
    PIMAGE_NT_HEADERS64 nt = reinterpret_cast<PIMAGE_NT_HEADERS64>(game_base + dos->e_lfanew);
    
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || nt->Signature != IMAGE_NT_SIGNATURE)
        return "Invalid PE headers";
    
    uintptr_t image_end = game_base + nt->OptionalHeader.SizeOfImage;
    std::ostringstream errors;
    
    for (int i = 0; i < FUNCTIONS_TOTAL; i++)
    {
        known_function_id func_id = static_cast<known_function_id>(i);
        uintptr_t address = game_base + known_function_offsets[func_id];
        const char* signature = known_function_signatures[func_id];
        const char* name = known_function_id_to_string(func_id);
        
        Signature sig(signature);
        
        if (address < game_base || address + sig.length > image_end)
        {
            errors << name << ": Out of bounds (0x" << std::hex << address << std::dec << ")\n";
            continue;
        }
        
        ValidationResult result = sig.validate(address);
        
        if (result != ValidationResult::SUCCESS)
        {
            errors << name << ": ";
            switch (result)
            {
            case ValidationResult::INVALID_ADDRESS:
                errors << "Invalid address (0x" << std::hex << address << std::dec << ")\n";
                break;
            case ValidationResult::SIGNATURE_MISMATCH:
                errors << "Signature mismatch (0x" << std::hex << address << std::dec << ")\n";
                break;
            default:
                errors << "Unknown error\n";
                break;
            }
        }
    }
    
    return errors.str();
}

ScanResult scan_all_signatures(uintptr_t game_base, std::atomic<float>& progress)
{
    PIMAGE_DOS_HEADER dos = reinterpret_cast<PIMAGE_DOS_HEADER>(game_base);
    PIMAGE_NT_HEADERS64 nt = reinterpret_cast<PIMAGE_NT_HEADERS64>(game_base + dos->e_lfanew);
    
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || nt->Signature != IMAGE_NT_SIGNATURE)
        return {};
    
    uintptr_t image_size = nt->OptionalHeader.SizeOfImage;
    
    ScanResult result;
    result.offsets = known_function_offsets;
    
    for (int i = 0; i < FUNCTIONS_TOTAL; i++)
    {
        progress.store(static_cast<float>(i) / static_cast<uint32_t>(FUNCTIONS_TOTAL));
        
        known_function_id func_id = static_cast<known_function_id>(i);
        const char* signature = known_function_signatures[func_id];
        
        Signature sig(signature);
        auto found = sig.scan(game_base, image_size);
        
        if (found.has_value())
        {
            result.offsets[func_id] = found.value() - game_base;
            result.found_count++;
        }
        else
        {
            result.offsets[func_id] = 0;
        }
    }
    
    progress.store(1.0f);
    
    result.success = (result.found_count == FUNCTIONS_TOTAL);
    return result;
}

