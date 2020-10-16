add_subdirectory("freetype")
add_subdirectory("glad")
add_subdirectory("glfw")
add_subdirectory("glm")

### lodepng ###
add_library(lodepng STATIC "lodepng/lodepng.h" "lodepng/lodepng.cpp")
target_include_directories(lodepng PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/lodepng")
###############

add_subdirectory("lua")
add_subdirectory("sae")
add_subdirectory("sae/lua")

