set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SAEEngine_Example_LuaAPIUsage "example.cpp")
target_link_libraries(SAEEngine_Example_LuaAPIUsage PRIVATE SAEEngine)

if(SAE_ENGINE_INSTALL_EXAMPLES)
	install(TARGETS SAEEngine_Example_LuaAPIUsage DESTINATION "examples/luaAPIUsage")
	file(GLOB files "*.lua")
	foreach(file ${files})
		install(FILES "${file}" DESTINATION "examples/luaAPIUsage")
	endforeach()
endif()