#include "engine_io.h"

#include <istream>
#include <ostream>
#include <string>

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

	// engine.io.print(string)
	int io_print(lua_State* _lua)
	{
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
		return 0;
	};

	// engine.io.println(string)
	int io_println(lua_State* _lua)
	{
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
		return 0;
	};

	// engine.io.read(count) -> string
	int io_read(lua_State* _lua)
	{
		size_t _count = (size_t)lua_tointeger(_lua, -1);
		auto _istr = lua_getistream(_lua);
		std::string _str{};
		if (_istr)
		{
			_str.resize(_count);
			_istr->read(_str.data(), (std::streamsize)_count);
		};
		lua_pushstring(_lua, _str.c_str());
		return 1;
	}

	// engine.io.readln() -> string
	int io_readln(lua_State* _lua)
	{
		auto _istr = lua_getistream(_lua);
		std::string _str{};
		if (_istr)
		{
			(*_istr) >> _str;
		};
		lua_pushstring(_lua, _str.c_str());
		return 1;
	};


	const luaL_Reg engine_io_lib[] =
	{
		luaL_Reg{ "print", &io_print },
		luaL_Reg{ "println", &io_println },
		luaL_Reg{ "read", &io_read },
		luaL_Reg{ "readln", &io_readln },

		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_io(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, engine_io_lib, 0);
		return 1;
	};

}