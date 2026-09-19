cmake_minimum_required(VERSION 3.20)
project(MK12Customizer VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find MinHook [citation:7]
find_path(MINHOOK_INCLUDE_DIR MinHook.h)
find_library(MINHOOK_LIBRARY minhook)

add_library(MK12Customizer SHARED
    src/dllmain.cpp
    src/core/HookManager.cpp
    src/core/PatternScanner.cpp
    src/hooks/FNameHook.cpp
    src/hooks/SignatureCheck.cpp
    src/features/AnnouncerSwap.cpp
    src/features/LightningColor.cpp
    src/config/ConfigManager.cpp
)

target_include_directories(MK12Customizer PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${MINHOOK_INCLUDE_DIR}
)

target_link_libraries(MK12Customizer PRIVATE
    ${MINHOOK_LIBRARY}
    psapi
)

set_target_properties(MK12Customizer PROPERTIES
    PREFIX ""
    SUFFIX ".ehp"  # EHP plugin format for MK1Hook [citation:4]
)
