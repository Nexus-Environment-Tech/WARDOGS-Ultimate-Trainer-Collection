// MK12Customizer — FName/FString Hooking
// Intercepts Unreal Engine's FName-to-string conversion for asset swapping [citation:4]

#include "FNameHook.h"
#include "core/HookManager.h"
#include "utils/Log.h"
#include <windows.h>
#include <MinHook.h>

namespace MK12 {

using FNameToString_t = const char* (*)(uint32_t nameIndex, uint32_t nameNumber);
static FNameToString_t oFNameToString = nullptr;

static std::unordered_map<std::string, std::string> g_StringSwaps;

static const char* hkFNameToString(uint32_t nameIndex, uint32_t nameNumber) {
    const char* result = oFNameToString(nameIndex, nameNumber);

    if (!result) return result;

    std::string original(result);

    auto it = g_StringSwaps.find(original);
    if (it != g_StringSwaps.end()) {
        Log::Info("FName swap: " + original + " -> " + it->second);
        static std::vector<std::string> s_SwapStorage;
        s_SwapStorage.push_back(it->second);
        return s_SwapStorage.back().c_str();
    }

    return result;
}

bool FNameHook::Initialize() {
    // Find FName::ToString using pattern scanning [citation:4]
    uintptr_t fnameToStringAddr = HookManager::Get().FindPattern(
        "MK12-Win64-Shipping.exe",
        "48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B 05"  // Example pattern
    );

    if (!fnameToStringAddr) {
        Log::Error("Failed to find FName::ToString");
        return false;
    }

    if (MH_CreateHook(reinterpret_cast<LPVOID>(fnameToStringAddr),
                      &hkFNameToString,
                      reinterpret_cast<LPVOID*>(&oFNameToString)) != MH_OK) {
        Log::Error("Failed to create FName hook");
        return false;
    }

    if (MH_EnableHook(reinterpret_cast<LPVOID>(fnameToStringAddr)) != MH_OK) {
        Log::Error("Failed to enable FName hook");
        return false;
    }

    LoadStringSwaps("string_swaps.txt");

    Log::Info("FName hook installed");
    return true;
}

void FNameHook::LoadStringSwaps(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        Log::Warn("string_swaps.txt not found: " + filepath);
        return;
    }

    std::string line;
    while (std::getline(file, line)) {
        if (line.empty() || line[0] == '#') continue;

        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string original = line.substr(0, pos);
            std::string swapped = line.substr(pos + 1);

            original.erase(0, original.find_first_not_of(" \t"));
            original.erase(original.find_last_not_of(" \t") + 1);
            swapped.erase(0, swapped.find_first_not_of(" \t"));
            swapped.erase(swapped.find_last_not_of(" \t") + 1);

            g_StringSwaps[original] = swapped;
            Log::Info("Registered swap: " + original + " -> " + swapped);
        }
    }
}

void FNameHook::Shutdown() {
    MH_DisableHook(MH_ALL_HOOKS);
    MH_Uninitialize();
    g_StringSwaps.clear();
}

} // namespace MK12
