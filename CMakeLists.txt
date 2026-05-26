cmake_minimum_required(VERSION 3.21)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_ARCHITECTURES "x86_64")

project(NoclipToggle VERSION 1.0.0)

# ── Locate Geode SDK ──────────────────────────────────────────────────────────
# Set GEODE_SDK environment variable to your SDK path, e.g.:
#   export GEODE_SDK=~/.geode-sdk   (macOS/Linux)
#   $env:GEODE_SDK = "C:\geode-sdk" (Windows PowerShell)
if (NOT DEFINED ENV{GEODE_SDK})
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.geode-sdk")
        set(ENV{GEODE_SDK} "${CMAKE_CURRENT_SOURCE_DIR}/.geode-sdk")
    else()
        message(FATAL_ERROR
            "Unable to find Geode SDK! Please set the GEODE_SDK "
            "environment variable to the path of your Geode SDK."
        )
    endif()
endif()

add_subdirectory($ENV{GEODE_SDK} ${CMAKE_CURRENT_BINARY_DIR}/geode)

# ── Mod target ────────────────────────────────────────────────────────────────
add_library(${PROJECT_NAME} SHARED
    src/main.cpp
)

target_link_libraries(${PROJECT_NAME} geode-sdk)

# ── Package the .geode file ───────────────────────────────────────────────────
setup_geode_mod(${PROJECT_NAME})
