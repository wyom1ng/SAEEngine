#pragma once

#include <lua.hpp>

#include <new>
#include <concepts>
#include <type_traits>
#include <typeinfo>
#include <atomic>

namespace sae::lua
{

	/**
	 * @brief Creates a new class meta table associated with name _tname and leaves onto the top of the stack,
	 * if the name is already used, still pushes the table onto the top of the stack
	 * @param _lua lua state
	 * @param _tname name of the class
	 * @return 
	*/
	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions, int _upVals);
	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions);
	void lua_newclass(lua_State* _lua, const char* _tname);
	
	

	






	int lua_inherit(lua_State* _lua, const char* _tname, int _idx);


	bool lua_isbaseof(lua_State* _lua, int _idx, const char* _baseName);


	void* lua_downcast(lua_State* _lua, int _idx, const char* _baseName);

	template <typename T> requires std::is_default_constructible<T>::value
	T* lua_newudata(lua_State* _lua)
	{
		return new (lua_newuserdata(_lua, sizeof(T))) T{};
	};

	template <typename T, typename... CArgs>
	T* lua_newudata(lua_State* _lua, CArgs... _arg)
	{
		return new (lua_newuserdata(_lua, sizeof(T))) T{ _arg... };
	};

	template <typename T> requires std::is_default_constructible<T>::value
	T* lua_newinstance(lua_State* _lua, const char* _parentName)
	{
		auto _out = lua_newudata<T>(_lua);
		luaL_getmetatable(_lua, _parentName);
		lua_setmetatable(_lua, -2);
		return _out;
	};
	template <typename T, typename... CArgs>
	T* lua_newinstance(lua_State* _lua, const char* _parentName, CArgs... _arg)
	{
		auto _out = lua_newudata<T, CArgs...>(_lua, _arg...);
		luaL_getmetatable(_lua, _parentName);
		lua_setmetatable(_lua, -2);
		return _out;
	};

	template <typename T>
	T* lua_toinstance(lua_State* _lua, int _idx, const char* _tname)
	{
		void* ud = lua::lua_downcast(_lua, _idx, _tname);
		if (ud == nullptr)
		{
			luaL_typeerror(_lua, _idx, _tname);
		};
		return (T*)ud;
	};





	int lua_getlen(lua_State* _lua, int _idx);





	

};