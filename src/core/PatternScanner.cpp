// MK12Customizer — Pattern Scanner
// Implements IDA-style wildcard pattern scanning [citation:4]

#include "PatternScanner.h"
#include "utils/Log.h"
#include <windows.h>
#include <psapi.h>

namespace MK12 {

// Convert pattern string "80 B9 ? ? ? ? 00 49 8B" to byte array + mask
static std::vector<std::pair<uint8_t, bool>> ParsePattern(const std::string& pattern) {
    std::vector<std::pair<uint8_t, bool>> result;
    size_t i = 0;

    while (i < pattern.size()) {
        if (pattern[i] == ' ') {
            i++;
            continue;
        }

        if (pattern[i] == '?') {
            result.push_back({0x00, false}); // wildcard
            i++;
        } else {
            std::string hex = pattern.substr(i, 2);
            uint8_t byte = static_cast<uint8_t>(std::stoul(hex, nullptr, 16));
            result.push_back({byte, true});
            i += 2;
        }
    }

    return result;
}

uintptr_t PatternScanner::Scan(const std::string& moduleName, const std::string& pattern) {
    HMODULE hModule = GetModuleHandleA(moduleName.c_str());
    if (!hModule) {
        Log::Error("Module not found: " + moduleName);
        return 0;
    }

    MODULEINFO modInfo{};
    if (!GetModuleInformation(GetCurrentProcess(), hModule, &modInfo, sizeof(modInfo))) {
        Log::Error("Failed to get module info");
        return 0;
    }

    uintptr_t base = reinterpret_cast<uintptr_t>(modInfo.lpBaseOfDll);
    size_t size = modInfo.SizeOfImage;

    auto parsed = ParsePattern(pattern);
    if (parsed.empty()) {
        Log::Error("Invalid pattern: " + pattern);
        return 0;
    }

    // Scan memory
    for (size_t i = 0; i < size - parsed.size(); i++) {
        bool found = true;

        for (size_t j = 0; j < parsed.size(); j++) {
            if (parsed[j].second) {
                uint8_t byte = *reinterpret_cast<uint8_t*>(base + i + j);
                if (byte != parsed[j].first) {
                    found = false;
                    break;
                }
            }
        }

        if (found) {
            uintptr_t address = base + i;
            Log::Info("Pattern found at 0x" + std::to_string(address));
            return address;
        }
    }

    Log::Warn("Pattern not found: " + pattern);
    return 0;
}

} // namespace MK12
