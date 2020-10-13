#pragma once

#include "SAEEngine.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <filesystem>


namespace sae::engine
{
	struct glShaderError
	{
		GLuint result = 0;
		std::string info_log{};
	};

	struct Shader_Data
	{
	public:
		
		const std::string& name() const noexcept;
		GLuint id() const noexcept;

		bool good() const noexcept;
		explicit operator bool() const noexcept;

		void bind() const;
		void unbind() const;

		void destroy();

		Shader_Data(const std::string& _name, const std::filesystem::path& _vertexPath, const std::filesystem::path& _fragmentPath);
		~Shader_Data();
	private:
		GLuint id_ = 0;
		const std::string name_;
	};







	Shader_Data* lua_toshader(lua_State* _lua, int _idx, int _arg);


	// engine.shader.new(name, vertexPath, fragmentPath) -> Shader_Data
	int shader_new(lua_State* _lua);

	// shader:good() -> bool
	int shader_good(lua_State* _lua);

	// shader:name() -> string
	int shader_name(lua_State* _lua);

	// shader:destroy()
	int shader_destroy(lua_State* _lua);

	// shader:__gc()
	int shader_destructor(lua_State* _lua);


	static inline const luaL_Reg shader_funcs[] =
	{
		luaL_Reg{ "new", &shader_new },
		luaL_Reg{ "good", &shader_good },
		luaL_Reg{ "name", &shader_name },
		luaL_Reg{ "destroy", &shader_destroy },
		luaL_Reg{ "__gc", &shader_destructor },

		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_shader(lua_State* _lua);
}