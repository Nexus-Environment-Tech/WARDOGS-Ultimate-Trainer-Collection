// MK12Customizer — Hook Manager
// Pattern scanning and memory patching for Mortal Kombat 1 [citation:4]

#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

namespace MK12 {

struct Pattern {
    std::string name;
    std::string signature;      // IDA-style pattern with wildcards
    uintptr_t address = 0;
    bool resolved = false;
};

class HookManager {
public:
    static HookManager& Get();

    bool Initialize();
    void Shutdown();

    // Pattern scanning [citation:4]
    uintptr_t FindPattern(const std::string& moduleName, const std::string& pattern);
    bool ResolvePattern(const std::string& name);

    // Signature patch (Anti-Cheat Bypass) [citation:4]
    bool ApplySignaturePatch();

    // FName hooking for asset swapping [citation:4]
    bool HookFNameConversion();

    // Memory patching [citation:4]
    bool PatchBytes(uintptr_t address, const std::vector<uint8_t>& bytes);
    bool NopBytes(uintptr_t address, size_t count);

private:
    HookManager() = default;
    ~HookManager() = default;

    std::unordered_map<std::string, Pattern> patterns_;
    bool initialized_ = false;
};

} // namespace MK12
