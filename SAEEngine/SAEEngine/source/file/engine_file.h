#pragma once

#include "SAEEngine.h"


#include <filesystem>
#include <fstream>

namespace sae::engine
{

	// fs.current_path() -> path
	int fs_current_path(lua_State* _lua);

	// fs.dofile(scriptPath)
	int fs_dofile(lua_State* _lua, std::nothrow_t) noexcept;

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	// fs.dofile(scriptPath)
	int fs_dofile(lua_State* _lua);
#else
	// fs.dofile(scriptPath)
	int fs_dofile(lua_State* _lua) noexcept;
#endif

	const auto NEXT_SCRIPT_KEY = "SAEEngine.next_script";

	// fs.doAfter(scriptPath)
	int fs_doAfter(lua_State* _lua);




	




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

	constexpr static inline auto PATH_DATA_TNAME = "SAEEngine.Path_Data";

	Path_Data* lua_topathdata(lua_State* _lua, int _idx);

	// fs.path.new(string) -> Path_Data
	int fs_path_new(lua_State* _lua);




	// fs.path:__tostring() -> string
	int fs_path_tostring(lua_State* _lua);

	// fs.path:__gc()
	int fs_path_destructor(lua_State* _lua);



	const luaL_Reg fs_path_lib[] =
	{
		luaL_Reg{ "new", &fs_path_new },




		luaL_Reg{ "__tostring", &fs_path_tostring },
		luaL_Reg{ "__gc", &fs_path_destructor },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_fs_path(lua_State* _lua);






	const luaL_Reg fs_lib[] =
	{
		luaL_Reg{ "current_path", &fs_current_path },
		luaL_Reg{ "dofile", &fs_dofile },
		luaL_Reg{ "doAfter", &fs_doAfter },

		luaL_Reg{ NULL, NULL }
	};



	int luaopen_engine_fs(lua_State* _lua);

}