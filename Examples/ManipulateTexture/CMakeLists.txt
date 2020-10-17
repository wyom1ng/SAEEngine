set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(SAEEngine_Example_ManipulateTexture "manipulateTexture.cpp")
target_link_libraries(SAEEngine_Example_ManipulateTexture PRIVATE SAEEngine)

if(SAE_ENGINE_INSTALL_EXAMPLES)
install(TARGETS SAEEngine_Example_ManipulateTexture DESTINATION "examples/textureManipulation")
	file(GLOB files "*.lua")
	foreach(file ${files})
		install(FILES "${file}" DESTINATION "examples/textureManipulation")
	endforeach()
endif()