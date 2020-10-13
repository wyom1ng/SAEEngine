#pragma once

#include "engine_window.h"

#include <new>


#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
#include <iostream>
#endif

#include <map>

namespace sae::engine
{
	GLFWwindow* Window::get() const noexcept { return this->window_; };

	Window::operator GLFWwindow*& () noexcept { return this->window_; };
	Window::operator GLFWwindow* const& () const noexcept { return this->window_; };

	bool Window::operator==(const Window& _other) const noexcept { return (this->window_ == _other.window_); };
	bool Window::operator!=(const Window& _other) const noexcept { return (this->window_ != _other.window_); };

	bool Window::operator==(GLFWwindow* _window) const noexcept { return (this->window_ == _window); };
	bool Window::operator!=(GLFWwindow* _window) const noexcept { return (this->window_ != _window); };

	bool Window::good() const noexcept { return (this->window_ != nullptr); };
	void Window::close()
	{
		if (this->good())
		{
			glfwDestroyWindow(this->window_);
			this->window_ = nullptr;
		};
	};

	Window::operator bool() const { return this->good(); };


	void Window::update()
	{
		glfwSwapBuffers(this->window_);
		glfwPollEvents();
	};

	Window::Window(int _width, int _height, const char* _title, std::optional<int> _monitor, std::optional<GLFWwindow*> _share)
	{
		GLFWmonitor* _useMonitor = nullptr;
		if (_monitor)
		{
			auto _mcount = 0;
			auto _mptr = glfwGetMonitors(&_mcount);
			if (*_monitor >= _mcount)
			{
				if (_mcount != 0)
				{
					_useMonitor = _mptr[0];
				}
				else
				{
#ifdef SAE_ENGINE_USE_EXCEPTIONS
					throw std::out_of_range{ "no monitors were found" };
#else
					return;
#endif
				};
			}
			else
			{
				_useMonitor = _mptr[*_monitor];
			};
		};

		GLFWwindow* _shareWindow = nullptr;
		if (_share)
			_shareWindow = *_share;

		this->window_ = glfwCreateWindow(_width, _height, _title, _useMonitor, _shareWindow);
		if (this->good())
		{
			glfwMakeContextCurrent(this->window_);
			gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		}
		else
		{
#ifdef SAE_ENGINE_USE_EXCEPTIONS
			throw std::runtime_error{ "glfwCreateWindow() failed" };
#else
			return;
#endif
		};

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glViewport(0, 0, _width, _height);

	};

	Window::Window(int _width, int _height, const char* _title) :
		Window{ _width, _height, _title, std::nullopt, std::nullopt }
	{};

	Window::Window(Window&& other) noexcept :
		window_{ std::exchange(other.window_, nullptr) }
	{};
	Window& Window::operator=(Window&& other) noexcept
	{
		this->window_ = std::exchange(other.window_, nullptr);
		return *this;
	};

	Window::~Window()
	{
		this->close();
	};

}

namespace sae::engine
{

	void window_data::key_callback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);
				
		auto _ref = _wData->get_lua_callback("key");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _key);
			lua_pushinteger(_lua, _scancode);
			lua_pushinteger(_lua, _action);
			lua_pushinteger(_lua, _mods);
			auto _err = lua_safecall(_lua, 4, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};

	};
	void window_data::char_callback(GLFWwindow* _window, unsigned int _codepoint)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		auto _ref = _wData->get_lua_callback("char");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _codepoint);
			auto _err = lua_safecall(_lua, 1, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};
	};

	void window_data::mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		auto _ref = _wData->get_lua_callback("mouse");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _button);
			lua_pushinteger(_lua, _action);
			lua_pushinteger(_lua, _mods);
			auto _err = lua_safecall(_lua, 3, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};
	};
	void window_data::cursor_callback(GLFWwindow* _window, double _x, double _y)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		auto _ref = _wData->get_lua_callback("cursor");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _x);
			lua_pushinteger(_lua, _y);
			auto _err = lua_safecall(_lua, 2, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};
	};


	void window_data::close_callback(GLFWwindow* _window)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		auto _ref = _wData->get_lua_callback("close");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			auto _err = lua_safecall(_lua, 0, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};

	};
	void window_data::focus_callback(GLFWwindow* _window, int _focused)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		auto _ref = _wData->get_lua_callback("focus");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _focused);
			auto _err = lua_safecall(_lua, 1, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};
	};
	void window_data::resize_callback(GLFWwindow* _window, int _width, int _height)
	{
		auto _wData = (window_data*)glfwGetWindowUserPointer(_window);
		assert(_wData);

		_wData->active_scene()->set_display_size(_width, _height);
		glViewport(0, 0, _width, _height);

		auto _ref = _wData->get_lua_callback("resize");
		if (_ref)
		{
			auto _lua = _wData->lua_;
			auto _beginTop = lua_gettop(_lua);

			lua_rawgeti(_lua, LUA_REGISTRYINDEX, *_ref);
			assert(lua_isfunction(_lua, -1));
			lua_pushinteger(_lua, _width);
			lua_pushinteger(_lua, _height);
			auto _err = lua_safecall(_lua, 2, 0, 0);

			auto _endTop = lua_gettop(_lua);
			assert(_beginTop == _endTop);

		};

	};


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

	void window_data::push(Shader_Data* _shader)
	{
		this->shaders_.insert({ _shader->name(), _shader });
	};
	void window_data::erase(Shader_Data* _shader)
	{
		this->shaders_.erase(_shader->name());
	};

	window_data::~window_data()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~window_data()\n";
#endif
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
		auto _w = glfwGetCurrentContext();
		if (this->window_ != _w)
			glfwMakeContextCurrent(this->window_);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		auto _s = this->active_scene();
		if (_s)
		{
			_s->draw();
		};

		this->window_.update();
	};

#pragma region WINDOW_DATA_CONSTRUCTOR
	window_data::window_data(Window _window, lua_State* _lua) :
		window_{ std::move(_window) },
		lua_{ _lua }
	{

		glfwSetMouseButtonCallback(this->window_, &mouse_button_callback);
		glfwSetCursorPosCallback(this->window_, &cursor_callback);

		glfwSetKeyCallback(this->window_, &key_callback);
		glfwSetCharCallback(this->window_, &char_callback);


		glfwSetWindowCloseCallback(this->window_, &close_callback);
		glfwSetWindowFocusCallback(this->window_, &focus_callback);
		glfwSetFramebufferSizeCallback(this->window_, &resize_callback);

	};
#pragma endregion WINDOW_DATA_CONSTRUCTOR


	bool window_data::is_valid_callback_key(const std::string& _key) const
	{
		return this->lua_callbacks_.contains(_key);
	};

	void window_data::set_lua_callback(const std::string& _key, int _ref)
	{
		this->lua_callbacks_.at(_key) = _ref;
	};
	void window_data::clear_lua_callback(const std::string& _key)
	{
		this->lua_callbacks_.at(_key) = std::nullopt;
	};
	const std::optional<int>& window_data::get_lua_callback(const std::string& _key) const
	{
		return this->lua_callbacks_.at(_key);
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
		auto _scene = lua_toscenedata(_lua, 1, 1);
		auto _beginTop = lua_gettop(_lua);
		
		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		auto _window = lua_towindow(_lua, -1, 0);

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

		int _w = 0;
		int _h = 0;
		glfwGetFramebufferSize(_window->window_, &_w, &_h);
		_scene->set_display_size(_w, _h);

		_scene->activate();
		lua_pushvalue(_lua, 1);
		lua_seti(_lua, t, (n + 1));

		assert(n == (lua::lua_getlen(_lua, t) - 1));

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

		if (n >= 1)
		{
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
		auto t = lua_gettop(_lua);
		lua_getfield(_lua, -1, "window");
		lua_getfield(_lua, -1, "scenes");
		lua_len(_lua, -1);
		lua_replace(_lua, t);
		lua_settop(_lua, t);

		auto _endtop = lua_gettop(_lua);
		assert(_beginTop == (_endtop - 1));
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

	// window.push_shader(Shader_Data)
	int window_push_shader(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);
		auto _ptr = lua_toshader(_lua, 1, 1);

		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		auto _window = lua_towindow(_lua, -1, 0);
		
		lua_getfield(_lua, -1, "shaders");

		lua_pushvalue(_lua, 1);
		lua_setfield(_lua, -2, _ptr->name().c_str());

		_window->push(_ptr);

		lua_pop(_lua, 3);
		auto _newTop = lua_gettop(_lua);

		assert(_beginTop == _newTop);

		return 0;
	};
	
	// window.get_shader(name) -> Shader_Data
	int window_get_shader(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);
		auto _name = lua_tostring(_lua, 1);

		lua_getglobal(_lua, "SAEEngine");
		auto t = lua_gettop(_lua);
		lua_getfield(_lua, -1, "window");
		lua_getfield(_lua, -1, "shaders");
		lua_getfield(_lua, -1, _name);
		lua_replace(_lua, t);
		lua_settop(_lua, t);

		auto _newTop = lua_gettop(_lua);
		assert(_beginTop == (_newTop - 1));

		return 1;
	};

	// window.erase_shader(Shader_Data)
	int window_erase_shader(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);
		auto _ptr = lua_toshader(_lua, 1, 1);

		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		auto _window = lua_towindow(_lua, -1, 0);

		lua_getfield(_lua, -1, "shaders");
		lua_pushnil(_lua);
		lua_setfield(_lua, -2, _ptr->name().c_str());

		_window->erase(_ptr);

		lua_pop(_lua, 3);
		auto _newTop = lua_gettop(_lua);

		assert(_beginTop == _newTop);

		return 0;
	};

	// window.set_callback(name, function)
	int window_set_callback(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);

		luaL_argcheck(_lua, lua_isstring(_lua, 1), 1, "'string' expected");
		luaL_argcheck(_lua, (lua_isfunction(_lua, 2) || lua_isnil(_lua, 2)), 2, "'function' or 'nil' expected");

		static thread_local std::string _name{};
		_name = lua_tostring(_lua, 1);

		lua_getglobal(_lua, "SAEEngine");
		lua_getfield(_lua, -1, "window");
		auto _window = lua_towindow(_lua, -1, 0);
		lua_pop(_lua, 2);

		if (!_window->is_valid_callback_key(_name))
		{
			lua_settop(_lua, _beginTop);
			return luaL_argerror(_lua, 1, "invalid callback name");
		};

		if (lua_isnil(_lua, -1))
		{
			auto _ref = _window->get_lua_callback(_name);
			if (_ref)
			{
				luaL_unref(_lua, LUA_REGISTRYINDEX, *_ref);
				_window->clear_lua_callback(_name);
			};
			assert(!_window->get_lua_callback(_name).has_value());
		}
		else
		{
			lua_pushvalue(_lua, 2);
			auto _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
			_window->set_lua_callback(_name, _ref);
		};

		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == _endTop);

	};


	// window:__gc
	int window_destructor(lua_State* _lua)
	{
		auto _ptr = lua_towindow(_lua, -1, 1);
		_ptr->~window_data();
		return 0;
	};

	


	int luaopen_engine_window(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.window");

		luaL_setfuncs(_lua, window_lib_m, 0);

		lua_newtable(_lua);
		lua_setfield(_lua, -2, "scenes");

		lua_newtable(_lua);
		lua_setfield(_lua, -2, "shaders");

		lua_newtable(_lua);
		lua_setfield(_lua, -2, "textures");


		return 1;
	};


}