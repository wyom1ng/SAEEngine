set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SAEEngine_Example_UIExample "UIExample.cpp")
target_link_libraries(SAEEngine_Example_UIExample PRIVATE SAEEngine)
