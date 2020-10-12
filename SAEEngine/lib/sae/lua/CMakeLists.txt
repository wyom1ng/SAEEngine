cmake_minimum_required (VERSION 3.8)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project("SAELua" VERSION 0.1)

add_library(SAELua STATIC "include/SAELua.h" "source/SAELua.cpp")
target_include_directories(SAELua PUBLIC "include" PRIVATE "source")
target_link_libraries(SAELua liblua)