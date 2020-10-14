add_library(lodepng STATIC "lodepng.h" "lodepng.cpp")
target_include_directories(lodepng PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
