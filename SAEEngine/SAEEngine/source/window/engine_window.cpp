#pragma once

#include "engine_window.h"

#include <new>

namespace sae::engine
{
	window_data* lua_towindow(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = luaL_checkudata(_lua, _idx, "SAEEngine.window");
		luaL_argcheck(_lua, ud != NULL, _arg, "`window' expected");
		return (window_data*)ud;
	};
	window_data* lua_getwindow(lua_State* _lua)
	{
		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		auto _ptr = lua_towindow(_lua, -1, 1);
		lua_pop(_lua, 2);
		return _ptr;
	};

	// window.open(width, height, title, [monitor])
	int window_open(lua_State* _lua)
	{
		int _w = lua_tointeger(_lua, 1);
		int _h = lua_tointeger(_lua, 2);
		auto _title = lua_tostring(_lua, 3);
		std::optional<int> _monitor = std::nullopt;

		if (lua_gettop(_lua) >= 4)
		{
			_monitor = lua_tointeger(_lua, 4);
		};

		auto _ptr = new (lua_newuserdata(_lua, sizeof(window_data))) window_data
		{
			Window{ _w, _h, _title, _monitor, std::nullopt },
			_lua
		};

		glfwSetWindowUserPointer(_ptr->window_, _ptr);

		luaL_getmetatable(_lua, "SAEEngine.window");
		lua_setmetatable(_lua, -2);

		lua_getglobal(_lua, "SAEEngine");
		lua_pushvalue(_lua, -2);
		lua_setfield(_lua, -2, "window");
		lua_pop(_lua, 2);

		return 0;
	};

	// window.close()
	int window_close(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		_ptr->window_.close();
		return 0;
	};

	// window.get_size() -> width, height
	int window_get_size(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		int _w = 0;
		int _h = 0;
		glfwGetFramebufferSize(_ptr->window_, &_w, &_h);
		lua_pushinteger(_lua, _w);
		lua_pushinteger(_lua, _h);
		return 2;
	};

	// window.set_size(width, height)
	int window_set_size(lua_State* _lua)
	{
		int _w = lua_tointeger(_lua, -2);
		int _h = lua_tointeger(_lua, -1);
		auto _ptr = lua_getwindow(_lua);
		glfwSetWindowSize(_ptr->window_, _w, _h);
		return 0;
	};

	// window.iconify()
	int window_iconify(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		glfwIconifyWindow(_ptr->window_);
		return 0;
	};

	// window.attention()
	int window_attention(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		glfwRequestWindowAttention(_ptr->window_);
		return 0;
	};

	// window.hide()
	int window_hide(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		glfwHideWindow(_ptr->window_);
		return 0;
	};

	// window.show()
	int window_show(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		glfwShowWindow(_ptr->window_);
		return 0;
	};

	// window.focus()
	int window_focus(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		glfwFocusWindow(_ptr->window_);
		return 0;
	};

	// window:__gc
	int window_destructor(lua_State* _lua)
	{
		auto _ptr = lua_towindow(_lua, -1, 1);
		_ptr->~window_data();
		return 0;
	};



	static const luaL_Reg window_lib_f[] =
	{
		luaL_Reg{ NULL, NULL }
	};

	static const luaL_Reg window_lib_m[] =
	{
		luaL_Reg{ "__gc", &window_destructor },
		luaL_Reg{ "close", &window_close },
		luaL_Reg{ "get_size", &window_get_size},
		luaL_Reg{ "set_size", &window_set_size },
		luaL_Reg{ "iconify", &window_iconify},
		luaL_Reg{ "attention", &window_attention },
		luaL_Reg{ "show", &window_show },
		luaL_Reg{ "hide", &window_hide },
		luaL_Reg{ "focus", &window_focus },
		luaL_Reg{ "open", &window_open },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_window(lua_State* _lua)
	{
		luaL_newmetatable(_lua, "SAEEngine.window");

		lua_pushvalue(_lua, -1);
		lua_setfield(_lua, -2, "__index");

		luaL_setfuncs(_lua, window_lib_m, 0);

		lua_newtable(_lua);

		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "mouse");
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "cursor");

		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "key");
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "text");


		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "close");
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, "focus");

		lua_setfield(_lua, -2, "callback");

		return 1;
	};


}