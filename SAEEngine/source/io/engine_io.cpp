#include "engine_io.h"

#include <istream>
#include <ostream>
#include <string>
#include <cassert>

namespace sae::engine
{

	std::ostream* lua_getostream(lua_State* _lua)
	{
		lua_pushstring(_lua, "SAEEngine.ostream");
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		auto _ptr = (std::ostream*)lua_touserdata(_lua, -1);
		lua_pop(_lua, 1);
		return _ptr;
	};
	std::istream* lua_getistream(lua_State* _lua)
	{
		lua_pushstring(_lua, "SAEEngine.istream");
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		auto _ptr = (std::istream*)lua_touserdata(_lua, -1);
		lua_pop(_lua, 1);
		return _ptr;
	};

#if SAE_ENGINE_DEBUG_LEVEL > 0
#define LUACHECK_BEGIN(_lua) auto _beginTop = lua_gettop(_lua)
#define LUACHECK_END(_lua, _change) auto _endTop = lua_gettop(_lua); \
	assert((_beginTop + _change) == _endTop)
#else
#define LUACHECK_BEGIN(_lua) while(false){}
#define LUACHECK_END(_lua, _change) while(false){}
#endif

	// io.print(string)
	int lib_io::print(lua_State* _lua)
	{
		LUACHECK_BEGIN(_lua);

		auto t = lua_gettop(_lua);
		auto _ostr = lua_getostream(_lua);
		lua_settop(_lua, t);
		if (_ostr)
		{
			for (auto i = 1; i <= t; ++i)
			{
				(*_ostr) << lua_tostring(_lua, i);
			}
			_ostr->flush();
		};

		LUACHECK_END(_lua, 0);

		return 0;
	};

	// io.println(string)
	int lib_io::println(lua_State* _lua)
	{
		LUACHECK_BEGIN(_lua);

		auto t = lua_gettop(_lua);
		auto _ostr = lua_getostream(_lua);
		lua_settop(_lua, t);
		if (_ostr)
		{
			if (t != 0)
			{
				for (auto i = 1; i <= t; ++i)
				{
					(*_ostr) << lua_tostring(_lua, i) << '\n';
				}
			}
			else
			{
				(*_ostr) << '\n';
			};
			_ostr->flush();
		};

		LUACHECK_END(_lua, 0);

		return 0;
	};

	// io.read(count) -> string
	int lib_io::read(lua_State* _lua)
	{
		LUACHECK_BEGIN(_lua);

		size_t _count = (size_t)lua_tointeger(_lua, -1);
		auto _istr = lua_getistream(_lua);
		std::string _str{};
		if (_istr)
		{
			_str.resize(_count);
			_istr->read(_str.data(), (std::streamsize)_count);
		};
		lua_pushstring(_lua, _str.c_str());

		LUACHECK_END(_lua, 1);

		return 1;
	};

	// io.readln() -> string
	int lib_io::readln(lua_State* _lua)
	{
		LUACHECK_BEGIN(_lua);

		auto _istr = lua_getistream(_lua);
		std::string _str{};
		if (_istr)
		{
			(*_istr) >> _str;
		};
		lua_pushstring(_lua, _str.c_str());

		LUACHECK_END(_lua, 1);

		return 1;
	};


	int lib_io::lua_open(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);
		return 1;
	};

}