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






	

	struct lib_shader
	{
	public:
		
		struct ltype_Shader
		{
		private:

			// engine.shader.new(name, vertexPath, fragmentPath) -> Shader_Data
			static int new_f(lua_State* _lua);


			// shader:good() -> bool
			static int good(lua_State* _lua);

			// shader:name() -> string
			static int name(lua_State* _lua);

			// shader:destroy()
			static int destroy(lua_State* _lua);

			// shader:__gc()
			static int destructor(lua_State* _lua);


			constexpr static inline luaL_Reg funcs_f[] =
			{
				luaL_Reg{ "new", &new_f },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline luaL_Reg funcs_m[] =
			{
				luaL_Reg{ "good", &good },
				luaL_Reg{ "name", &name },
				luaL_Reg{ "destroy", &destroy },
				luaL_Reg{ "__gc", &destructor },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.shader";

		public:
			using value_type = Shader_Data;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _arg);

			constexpr static inline const char* tname() noexcept { return TYPENAME; };

			static int lua_open(lua_State* _lua);

		};



	private:



	public:

		static inline auto to_shader(lua_State* _lua, int _arg) { return ltype_Shader::to_userdata(_lua, _arg); };

		static int lua_open(lua_State* _lua);

	};

}