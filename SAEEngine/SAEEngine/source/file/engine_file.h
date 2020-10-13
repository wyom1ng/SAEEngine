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






	struct File_Data
	{

	};

	/*
		food for thought
	*/
	struct Path_Data
	{
	public:
		Path_Data(const std::filesystem::path& _path) :
			path_{ _path }
		{};
	private:
		std::filesystem::path path_{};
	};





	const luaL_Reg fs_lib[] =
	{
		luaL_Reg{ "current_path", &fs_current_path },
		luaL_Reg{ "dofile", &fs_dofile },
		luaL_Reg{ "doAfter", &fs_doAfter },

		luaL_Reg{ NULL, NULL }
	};



	int luaopen_engine_fs(lua_State* _lua);

}