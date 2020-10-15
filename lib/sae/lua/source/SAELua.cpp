#include "SAELua.h"

#include <cassert>

namespace sae::lua
{


	int lualib_tostring_f(lua_State* _lua)
	{
		if (lua_isstring(_lua, 1))
		{
			lua_pushnil(_lua);
			lua_rotate(_lua, 1, 1);
			assert(lua_isstring(_lua, -1));
			return 1;
		}
		else if (luaL_callmeta(_lua, -1, "__tostring"))
		{
			assert(lua_isstring(_lua, -1));
			return 1;
		}
		else
		{
			return luaL_argerror(_lua, 1, "not string or string convertible type");
		};
	};


	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions, int _upVals)
	{
		lua_pushstring(_lua, _tname);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		if (lua_isnoneornil(_lua, -1))
		{
			lua_pop(_lua, 1);

			lua_newtable(_lua);
			lua_pushvalue(_lua, -1);
			lua_setfield(_lua, -2, "__index");

			lua_pushstring(_lua, _tname);
			lua_pushvalue(_lua, -2);
			lua_settable(_lua, LUA_REGISTRYINDEX);

			lua_newtable(_lua);
			lua_pushvalue(_lua, -2);
			lua_setfield(_lua, -2, _tname);
			lua_setfield(_lua, -2, "__name");
			
			if(_functions)
				luaL_setfuncs(_lua, _functions, _upVals);

		};
	};
	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions)
	{
		lua_newclass(_lua, _tname, _functions, 0);
	};
	void lua_newclass(lua_State* _lua, const char* _tname)
	{
		lua_newclass(_lua, _tname, nullptr);
	};


	int lua_inherit(lua_State* _lua, const char* _tname, int _idx)
	{
		auto _beginTop = lua_gettop(_lua);

		if (_idx < 0)
			_idx = lua_gettop(_lua) + (_idx + 1);
		luaL_getmetatable(_lua, _tname);
		lua_pushvalue(_lua, -1);
		lua_setmetatable(_lua, _idx);

		lua_getfield(_lua, -1, "__gc");
		lua_setfield(_lua, _idx, "__gc");
		
		lua_getfield(_lua, -1, "__name");
		lua_getfield(_lua, _idx, "__name");

		lua_pushnil(_lua);
		while (lua_next(_lua, -3) != 0)
		{
			auto _key = lua_tostring(_lua, -2);
			lua_setfield(_lua, -3, _key);
		};
		
		lua_pop(_lua, 3);
		
		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == _endTop);

		return 0;
	};

	bool lua_isbaseof(lua_State* _lua, int _idx, const char* _baseName)
	{
		lua_getfield(_lua, _idx, "__name");
		lua_getfield(_lua, -1, _baseName);
		bool _out = true;
		if (lua_isnoneornil(_lua, -1))
		{
			return false;
		};
		lua_pop(_lua, 2);
		return _out;
	};

	void* lua_downcast(lua_State* _lua, int _idx, const char* _baseName)
	{
		void* _out = nullptr;
		if (lua_isbaseof(_lua, _idx, _baseName))
		{
			_out = lua_touserdata(_lua, _idx);
		};
		return _out;
	};


	int lua_getlen(lua_State* _lua, int _idx)
	{
		lua_len(_lua, _idx);
		auto n = lua_tointeger(_lua, -1);
		lua_pop(_lua, 1);
		return n;
	};
	


}


namespace sae::lua
{

	

}