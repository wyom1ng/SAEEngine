#include "engine_file.h"

#include "SAELua.h"

#include <string>
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




	int dofile(lua_State* _lua, const std::filesystem::path& _path, std::nothrow_t) noexcept
	{
		lua_gc(_lua, LUA_GCCOLLECT);
		auto _err = luaL_loadfile(_lua, _path.string().c_str());
		if (_err != LUA_OK)
		{
			return _err;
		};
		set_current_path(_lua, _path);
		return lua_safecall(_lua, 0, LUA_MULTRET, 0);
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	int dofile(lua_State* _lua, const std::filesystem::path& _path)
	{
		auto _beginTop = lua_gettop(_lua);
		lua_gc(_lua, LUA_GCCOLLECT);
		auto _err = luaL_loadfile(_lua, _path.string().c_str());
		if (_err != LUA_OK)
		{
			throw std::runtime_error{ "luaL_loadfile() error : " + std::to_string(_err) };
			return _err;
		};
		set_current_path(_lua, _path);
		auto _res = lua_safecall(_lua, 0, LUA_MULTRET, 0);

		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == _endTop);

		return _res;
	};
#else
	int dofile(lua_State* _lua, const std::filesystem::path& _path) noexcept
	{
		return dofile(_lua, _path, std::nothrow);
	};
#endif


}


namespace sae::engine
{

	// fs.path.new(string) -> Path_Data
	int lib_fs::ltype_Path::new_f(lua_State* _lua)
	{
		auto _path = lua_tostring(_lua, 1);
		auto _ptr = lua::lua_newinstance<Path_Data>(_lua, tname(), _path);
		return 1;
	};

	// fs.path:__tostring() -> string
	int lib_fs::ltype_Path::tostring(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		lua_pushstring(_lua, _ptr->get().string().c_str());
		return 1;
	};

	// fs.path:__gc()
	int lib_fs::ltype_Path::destructor(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		std::destroy_at(_ptr);
		return 0;
	};

	lib_fs::ltype_Path::pointer lib_fs::ltype_Path::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_fs::ltype_Path::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs_m, 0);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);

		return 1;
	};

}

namespace sae::engine
{

	// fs.current_path() -> path
	int lib_fs::current_path(lua_State* _lua)
	{
		lua_pushstring(_lua, CURRENT_PATH_KEY);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		assert(lua_isstring(_lua, -1));
		return 1;
	};




#ifdef SAE_ENGINE_USE_EXCEPTIONS
	// fs.dofile(scriptPath)
	int lib_fs::dofile(lua_State* _lua)
	{
		auto _path = lua_tostring(_lua, -1);
		return ::sae::engine::dofile(_lua, _path);
	};
#else
	// fs.dofile(scriptPath)
	int lib_fs::dofile(lua_State* _lua) noexcept
	{
		auto _path = lua_tostring(_lua, -1);
		return ::sae::engine::dofile(_lua, _path, std::nothrow);
	};
#endif

	// fs.doAfter(scriptPath)
	int lib_fs::doAfter(lua_State* _lua)
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


	int lib_fs::lua_open(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs, 0);

		assert(ltype_Path::lua_open(_lua) == 1);
		lua_setfield(_lua, -2, "path");

		return 1;
	};

}