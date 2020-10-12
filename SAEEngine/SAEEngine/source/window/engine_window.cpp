#pragma once

#include "engine_window.h"

#include <new>

namespace sae::engine
{




	window_data* lua_towindow(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.window");
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



	Scene_Data* window_data::active_scene() const
	{
		auto _lua = this->lua_;

		auto _beginTop = lua_gettop(_lua);

		lua_getglobal(_lua, "SAEEngine");
		if (lua_isnil(_lua, -1))
		{
			lua_pop(_lua, 1);
			return nullptr;
		};

		lua_getfield(_lua, -1, "window");
		if (lua_isnil(_lua, -1))
		{
			lua_pop(_lua, 2);
			return nullptr;
		};

		lua_getfield(_lua, -1, "scenes");
		if (lua_isnil(_lua, -1))
		{
			lua_pop(_lua, 3);
			return nullptr;
		};
		
		auto n = lua::lua_getlen(_lua, -1);
		Scene_Data* _out = nullptr;

		if (n > 0)
		{
			lua_rawgeti(_lua, -1, n);
			auto t = lua_gettop(_lua);
			_out = lua_toscenedata(_lua, t, 1);
			lua_pop(_lua, 1);
		};

		lua_pop(_lua, 3);
		assert(_beginTop == lua_gettop(_lua));

		return _out;

	};

	void window_data::update()
	{
		glClear(GL_COLOR_BUFFER_BIT);

		auto _s = this->active_scene();
		if (_s)
		{
			_s->draw();
		};

		this->window_.update();
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

	// window.push_scene(Scene_Data)
	int window_push_scene(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);
		
		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		lua_getfield(_lua, -1, "scenes");
		auto t = lua_gettop(_lua);

		auto n = lua::lua_getlen(_lua, -1);

		if (n > 0)
		{
			lua_geti(_lua, -1, n);
			auto _oldScene = lua_toscenedata(_lua, -1, 1);
			_oldScene->deactivate();
			lua_pop(_lua, 1);
		};

		auto _scene = lua_toscenedata(_lua, 1, 1);
		_scene->activate();
		lua_pushvalue(_lua, 1);
		lua_seti(_lua, t, (n + 1));

		lua_pop(_lua, 3);
		auto _newTop = lua_gettop(_lua);

		assert(_beginTop == _newTop);
		
		return 0;
	};

	// window.pop_scene()
	int window_pop_scene(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);

		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		lua_getfield(_lua, -1, "scenes");
		
		auto n = lua::lua_getlen(_lua, -1);

		lua_pushnil(_lua);
		lua_rawseti(_lua, -2, n);

		assert(lua::lua_getlen(_lua, -1) == (n - 1));
		--n;

		if (n >= 1)
		{
			lua_geti(_lua, -1, n);
			auto _scene = lua_toscenedata(_lua, -1, 1);
			_scene->activate();
			lua_pop(_lua, 1);
		};

		lua_pop(_lua, 3);
		assert(_beginTop == lua_gettop(_lua));

		return 0;
	};

	// window.scene_stack_size() -> integer
	int window_scene_stack_size(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);

		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		lua_getfield(_lua, -1, "scenes");

		lua_len(_lua, -1);
		lua_replace(_lua, -4);
		lua_settop(_lua, -4);

		assert(_beginTop == (lua_gettop(_lua) + 1));
		return 1;
	};

	// window.has_active_scene() -> bool
	int window_has_active_scene(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);

		window_get_size(_lua);
		auto n = lua_tointeger(_lua, -1);
		bool _out = false;
		if (n > 0)
			_out = true;
		lua_pop(_lua, 1);
		lua_pushboolean(_lua, _out);

		assert(_beginTop == (lua_gettop(_lua) + 1));

		return 1;
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

		luaL_Reg{ "push_scene", &window_push_scene },
		luaL_Reg{ "pop_scene", &window_pop_scene },
		luaL_Reg{ "get_scene_stack_size", &window_scene_stack_size },
		luaL_Reg{ "has_active_scene", &window_has_active_scene },

		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_window(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.window");

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

		lua_newtable(_lua);
		lua_setfield(_lua, -2, "scenes");


		return 1;
	};


}