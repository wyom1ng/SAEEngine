set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SAEEngine STATIC "source/SAEEngine.cpp" "include/SAEEngine.h" "source/gfx/engine_gfx.cpp" "source/scene/engine_scene.cpp" "source/LuaAPI.h" "source/window/engine_window.cpp" "source/io/engine_io.cpp" "source/os/engine_os.cpp" "source/io/engine_io.h" "source/scene/engine_scene.h" "source/os/engine_os.h" "source/window/engine_window.h" "source/gfx/engine_gfx.h")

target_include_directories(SAEEngine PUBLIC ${PROJECT_BINARY_DIR} "include" PRIVATE "source")
target_link_libraries(SAEEngine PUBLIC liblua glfw glm glad lodepng SAELib SAELua)