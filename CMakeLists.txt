cmake_minimum_required (VERSION 3.8)
project ("SAEEngine" VERSION 0.1)

option(SAE_ENGINE_OVERRIDE_GLFW_VERSION "override the version checking for GLFW" OFF)
option(SAE_ENGINE_OVERRIDE_LUA_VERSION "override the version checking for Lua" OFF)
option(SAE_ENGINE_USE_EXCEPTIONS "enable / disable exceptions without changing the API" ON)
option(SAE_ENGINE_HARD_ERRORS "causes errors to abort on site rather than throw an exception or return an error code" ON)

if(CMAKE_BUILD_TYPE MATCHES "Release")
	set(SAEEngine_DEBUG_LEVEL 0)
else()
	set(SAEEngine_DEBUG_LEVEL 1)
endif()

configure_file("SAEEngineConfig.h.in" "SAEEngineConfig.h")

add_definitions(
        -DPROJECT_ROOT="${CMAKE_CURRENT_LIST_DIR}"
)

add_subdirectory("lib")
add_subdirectory("SAEEngine")
add_subdirectory("Examples")
