set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SAEEngine_Example "example.cpp")
target_link_libraries(SAEEngine_Example PRIVATE SAEEngine)