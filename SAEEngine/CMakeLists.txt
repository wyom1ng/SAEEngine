cmake_minimum_required (VERSION 3.8)
project ("SAEEngine" VERSION 0.1)

option(SAE_ENGINE_OVERRIDE_GLFW_VERSION "override the version checking for GLFW" OFF)
option(SAE_ENGINE_OVERRIDE_LUA_VERSION "override the version checking for Lua" OFF)
option(SAE_ENGINE_USE_EXCEPTIONS "enable / disable exceptions without changing the API" ON)

configure_file("SAEEngineConfig.h.in" "SAEEngineConfig.h")

add_subdirectory("lib")
add_subdirectory("SAEEngine")
add_subdirectory("Examples")
