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
		return 1;
	};

}