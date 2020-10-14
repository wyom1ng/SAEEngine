set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SAEEngine STATIC "source/SAEEngine.cpp" "include/SAEEngine.h" "source/gfx/engine_gfx.cpp" "source/scene/engine_scene.cpp" "source/LuaAPI.h" "source/window/engine_window.cpp" "source/io/engine_io.cpp" "source/os/engine_os.cpp" "source/io/engine_io.h" "source/scene/engine_scene.h" "source/os/engine_os.h" "source/window/engine_window.h" "source/gfx/engine_gfx.h" "source/shader/engine_shader.cpp" "source/shader/engine_shader.h" "source/file/engine_file.h" "source/file/engine_file.cpp" "source/physics/engine_physics.h" "source/physics/engine_physics.cpp" "include/SAEEngine_BaseTypes.h" "source/texture/engine_texture.h" "source/texture/engine_texture.cpp" "source/texture/texture.cpp")

target_include_directories(SAEEngine PUBLIC ${PROJECT_BINARY_DIR} "include" PRIVATE "source")
target_link_libraries(SAEEngine PUBLIC liblua glfw glm glad SAELib SAELua PRIVATE lodepng)