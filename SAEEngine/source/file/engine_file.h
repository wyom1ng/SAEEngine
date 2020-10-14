#pragma once

#include "SAEEngine.h"


#include <filesystem>
#include <fstream>

namespace sae::engine
{

	/*
		food for thought
	*/
	struct Path_Data
	{
	public:

		const std::filesystem::path& get() const noexcept { return this->path_; };



		Path_Data(const std::filesystem::path& _path) :
			path_{ _path }
		{};
	private:
		std::filesystem::path path_{};
	};






	int dofile(lua_State* _lua, const std::filesystem::path& _path, std::nothrow_t) noexcept;
#ifdef SAE_ENGINE_USE_EXCEPTIONS
	int dofile(lua_State* _lua, const std::filesystem::path& _path);
#else
	int dofile(lua_State* _lua, const std::filesystem::path& _path) noexcept;
#endif



	struct lib_fs
	{
	public:

		struct ltype_Path
		{
		private:
			
			// fs.path.new(string) -> Path_Data
			static int new_f(lua_State* _lua);

			constexpr static inline luaL_Reg funcs_f[] =
			{
				luaL_Reg{ "new", &new_f },
				luaL_Reg{ NULL, NULL }
			};



			// fs.path:__tostring() -> string
			static int tostring(lua_State* _lua);

			// fs.path:__gc()
			static int destructor(lua_State* _lua);
			
	
			constexpr static inline luaL_Reg funcs_m[] =
			{
				luaL_Reg{ "__tostring", &tostring },
				luaL_Reg{ "__gc", &destructor },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.Path_Data";

		public:
			using value_type = Path_Data;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			constexpr static inline const char* tname() noexcept { return TYPENAME; };

			static int lua_open(lua_State* _lua);

		};


	private:

		// fs.current_path() -> path
		static int current_path(lua_State* _lua);

#ifdef SAE_ENGINE_USE_EXCEPTIONS
		// fs.dofile(scriptPath)
		static int dofile(lua_State* _lua);
#else
		// fs.dofile(scriptPath)
		static int dofile(lua_State* _lua) noexcept;
#endif

		// fs.doAfter(scriptPath)
		static int doAfter(lua_State* _lua);


		constexpr static inline luaL_Reg funcs[] =
		{
			luaL_Reg{ "current_path", &current_path },
			luaL_Reg{ "dofile", &dofile },
			luaL_Reg{ "doAfter", &doAfter },

			luaL_Reg{ NULL, NULL }
		};


	public:

		constexpr static inline auto NEXT_SCRIPT_KEY = "SAEEngine.next_script";

		static int lua_open(lua_State* _lua);

	};


}