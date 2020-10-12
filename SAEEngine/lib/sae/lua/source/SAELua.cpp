#include "SAELua.h"

namespace sae::lua
{

	void lua_newclass(lua_State* _lua, const char* _tname)
	{
		lua_pushstring(_lua, _tname);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		if (lua_isnoneornil(_lua, -1))
		{
			lua_pop(_lua, 1);

			lua_newtable(_lua);

			lua_pushstring(_lua, _tname);
			lua_pushvalue(_lua, -2);
			lua_settable(_lua, LUA_REGISTRYINDEX);

			lua_newtable(_lua);
			lua_pushvalue(_lua, -2);
			lua_setfield(_lua, -2, _tname);

			lua_setfield(_lua, -2, "__name");
		};
	};
	int lua_inherit(lua_State* _lua, const char* _tname, int _idx)
	{
		if (_idx < 0)
			_idx = lua_gettop(_lua) + (_idx + 1);
		luaL_getmetatable(_lua, _tname);
		lua_pushvalue(_lua, -1);
		lua_setmetatable(_lua, _idx);

		lua_getfield(_lua, -1, "__gc");
		lua_setfield(_lua, _idx, "__gc");

		lua_pop(_lua, 1);

		lua_getfield(_lua, _idx, "__name");
		luaL_getmetatable(_lua, _tname);
		lua_setfield(_lua, -2, _tname);


		lua_pop(_lua, 1);

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

}