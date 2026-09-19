// MK12Customizer — DLL Entry Point
// Based on architecture from MK12TTH (thethiny) and MK1Hook (ermaccer) [citation:4][citation:5]
// Licensed under PolyForm Noncommercial 1.0.0 [citation:9]

#include <windows.h>
#include <string>
#include "core/HookManager.h"
#include "config/ConfigManager.h"
#include "utils/Log.h"

static HMODULE g_hModule = nullptr;

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            g_hModule = hModule;
            DisableThreadLibraryCalls(hModule);

            // Initialize logging
            MK12::Log::Initialize();
            MK12::Log::Info("MK12Customizer loaded");

            // Load configuration
            MK12::ConfigManager::Get().Load("MK12Customizer.ini");

            // Initialize hook manager
            if (!MK12::HookManager::Get().Initialize()) {
                MK12::Log::Error("Failed to initialize hook manager");
                return FALSE;
            }

            // Apply signature patches (Anti-Cheat Bypass) [citation:4]
            if (MK12::ConfigManager::Get().GetBool("Patches", "bDisableSignatureCheck", true)) {
                MK12::HookManager::Get().ApplySignaturePatch();
            }

            // Apply FName hook for asset swapping [citation:4]
            if (MK12::ConfigManager::Get().GetBool("Hooks", "bEnableFNameHook", true)) {
                MK12::HookManager::Get().HookFNameConversion();
            }

            MK12::Log::Info("Initialization complete");
            break;
        }
        case DLL_PROCESS_DETACH: {
            MK12::HookManager::Get().Shutdown();
            MK12::Log::Shutdown();
            break;
        }
    }
    return TRUE;
}
