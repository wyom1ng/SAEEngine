set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_library(SAEEngine STATIC "source/SAEEngine.cpp" "include/SAEEngine.h" "source/GFXTypes.cpp")

target_include_directories(SAEEngine PUBLIC "include" PRIVATE "source")
target_link_libraries(SAEEngine PUBLIC liblua glfw glm glad lodepng SAELib)