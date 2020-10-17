set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SAEEngine_Example_Extension "ExtensionExample.cpp")
target_link_libraries(SAEEngine_Example_Extension PRIVATE SAEEngine)

if(SAE_ENGINE_INSTALL_EXAMPLES)
	install(TARGETS SAEEngine_Example_Extension DESTINATION "examples/customExtensions")
	file(GLOB files "*.lua")
	foreach(file ${files})
		install(FILES "${file}" DESTINATION "examples/customExtensions")
	endforeach()
endif()