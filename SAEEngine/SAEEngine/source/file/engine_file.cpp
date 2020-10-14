#include "engine_file.h"

#include "SAELua.h"

#include <cassert>
#include <filesystem>

namespace sae::engine
{

	namespace
	{
		const auto CURRENT_PATH_KEY = "SAEEngine.current_path";


		void set_current_path(lua_State* _lua, std::filesystem::path _path)
		{
			if (_path.has_extension())
				_path.remove_filename();

			auto _beginTop = lua_gettop(_lua);
			lua_pushstring(_lua, CURRENT_PATH_KEY);
			lua_pushstring(_lua, _path.string().c_str());
			lua_settable(_lua, LUA_REGISTRYINDEX);
			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);
		};
		
		std::filesystem::path get_current_path(lua_State* _lua)
		{
			lua_pushstring(_lua, CURRENT_PATH_KEY);
			std::filesystem::path _out{ lua_tostring(_lua, -1) };
			lua_pop(_lua, 1);
			return _out;
		};

	}



	int fs_dofile(lua_State* _lua, std::nothrow_t) noexcept
	{
		auto _path = lua_tostring(_lua, -1);
		auto _err = luaL_loadfile(_lua, _path);
		if (_err != LUA_OK)
		{
			return _err;
		};
		set_current_path(_lua, _path);
		return lua_safecall(_lua, 0, LUA_MULTRET, 0);
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	int fs_dofile(lua_State* _lua)
	{
		auto _path = lua_tostring(_lua, -1);
		auto _err = luaL_loadfile(_lua, _path);
		if (_err != LUA_OK)
		{
			throw std::runtime_error{ "luaL_loadfile()" };
			return _err;
		};
		set_current_path(_lua, _path);
		return lua_safecall(_lua, 0, LUA_MULTRET, 0);
	};
#else
	int fs_dofile(lua_State* _lua) noexcept
	{
		return fs_dofile(_lua, _path, std::nothrow);
	};
#endif

	// fs.doAfter(scriptPath)
	int fs_doAfter(lua_State* _lua)
	{
		if (!lua_isstring(_lua, -1))
		{
			luaL_argerror(_lua, 1, "'path' expected");
		};
		lua_pushstring(_lua, NEXT_SCRIPT_KEY);
		lua_pushvalue(_lua, -2);
		lua_settable(_lua, LUA_REGISTRYINDEX);
		return 0;
	};

	// fs.current_path() -> path
	int fs_current_path(lua_State* _lua)
	{
		lua_pushstring(_lua, CURRENT_PATH_KEY);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		assert(lua_isstring(_lua, -1));
		return 1;
	};
	







	int luaopen_engine_fs(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, fs_lib, 0);

		luaopen_fs_path(_lua);
		lua_setfield(_lua, -2, "path");

		return 1;
	};

}

namespace sae::engine
{

	Path_Data* lua_topathdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<Path_Data>(_lua, _idx, PATH_DATA_TNAME);
	};

	// fs.path.new(string) -> Path_Data
	int fs_path_new(lua_State* _lua)
	{
		auto _path = lua_tostring(_lua, 1);
		auto _ptr = lua::lua_newinstance<Path_Data>(_lua, PATH_DATA_TNAME, _path);
		return 1;
	};




	// fs.path:__tostring() -> string
	int fs_path_tostring(lua_State* _lua)
	{
		auto _ptr = lua_topathdata(_lua, 1);
		lua_pushstring(_lua, _ptr->get().string().c_str());
		return 1;
	};

	// fs.path:__gc()
	int fs_path_destructor(lua_State* _lua)
	{
		auto _ptr = lua_topathdata(_lua, 1);
		_ptr->~Path_Data();
		return 0;
	};



	int luaopen_fs_path(lua_State* _lua)
	{
		lua::lua_newclass(_lua, PATH_DATA_TNAME);
		luaL_setfuncs(_lua, fs_path_lib, 0);
		return 1;
	};


}