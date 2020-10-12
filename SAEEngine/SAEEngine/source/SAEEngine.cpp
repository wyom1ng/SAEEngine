#include "SAEEngine.h"

#include <new>
#include <type_traits>
#include <functional>

#include <stdexcept>
#include <optional>
#include <chrono>
#include <thread>

#include <istream>
#include <ostream>

namespace sae::engine
{
	lua_State* Lua::get() const noexcept { return this->lua_; };

	Lua::operator lua_State*& () noexcept { return this->lua_; };
	Lua::operator lua_State* const& () const noexcept { return this->lua_; };

	bool Lua::good() const noexcept { return this->lua_ != nullptr; };
	Lua::operator bool() const noexcept { return this->good(); };

	void Lua::close()
	{
		if (this->good())
		{
			lua_close(this->lua_);
			this->lua_ = nullptr;
		};
	};

	Lua::Lua() :
		lua_{ luaL_newstate() }
	{};
	Lua::Lua(lua_Alloc _f, void* _ud) :
		lua_{ lua_newstate(_f, _ud) }
	{};

	Lua::Lua(Lua&& other) noexcept :
		lua_{ std::exchange(other.lua_, nullptr) }
	{};
	Lua& Lua::operator=(Lua&& other) noexcept
	{
		this->lua_ = std::exchange(other.lua_, nullptr);
		return *this;
	};

	Lua::~Lua()
	{
		this->close();
	}
};

namespace sae::engine
{

	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f, std::nothrow_t) noexcept
	{
		return lua_pcall(_lua, _args, _rets, _f);
	};

#if SAE_ENGINE_USE_EXCEPTIONS
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f)
	{
		auto _err = lua_pcall(_lua, _args, _rets, _f);
		switch (_err)
		{
		case LUA_OK:
			break;
		case LUA_ERRRUN:
			throw std::runtime_error{ std::string{lua_tostring(_lua, -1)} };
		default:
			throw std::runtime_error{ "lua_pcall() error" };
		};
		return _err;
	};
#else
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f) noexcept
	{
		return lua_safecall(_lua, _args, _rets, _f, std::nothrow);
	};
#endif

	struct Window
	{
	public:

		GLFWwindow* get() const noexcept { return this->window_; };

		operator GLFWwindow*& () noexcept { return this->window_; };
		operator GLFWwindow* const& () const noexcept { return this->window_; };

		bool operator==(const Window& _other) const noexcept { return (this->window_ == _other.window_); };
		bool operator!=(const Window& _other) const noexcept { return (this->window_ != _other.window_); };

		bool operator==(GLFWwindow* _window) const noexcept { return (this->window_ == _window); };
		bool operator!=(GLFWwindow* _window) const noexcept { return (this->window_ != _window); };

		bool good() const noexcept { return (this->window_ != nullptr); };
		void close()
		{
			if (this->good())
			{
				glfwDestroyWindow(this->window_);
				this->window_ = nullptr;
			};
		};

		explicit operator bool() const { return this->good(); };


		void update()
		{
			glfwSwapBuffers(this->window_);
			glfwPollEvents();
		};

		Window(int _width, int _height, const char* _title, std::optional<int> _monitor, std::optional<GLFWwindow*> _share)
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
#if SAE_ENGINE_USE_EXCEPTIONS
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
#if SAE_ENGINE_USE_EXCEPTIONS
				throw std::runtime_error{ "glfwCreateWindow() failed" };
#else
				return;
#endif
			};
		};

		Window(int _width, int _height, const char* _title) :
			Window{ _width, _height, _title, std::nullopt, std::nullopt }
		{};

		Window(const Window& other) = delete;
		Window& operator=(const Window& other) = delete;

		Window(Window&& other) noexcept :
			window_{ std::exchange(other.window_, nullptr) }
		{};
		Window& operator=(Window&& other) noexcept
		{
			this->window_ = std::exchange(other.window_, nullptr);
		};

		~Window()
		{
			this->close();
		};

	private:
		GLFWwindow* window_ = nullptr;
	};


	struct WindowRef;
	struct LuaRef;

}


#pragma region WINDOW_LIB
namespace sae::engine
{

	struct window_data
	{

		static void key_callback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "key");

			if (lua_isfunction(_lua, -1))
			{
				lua_pushinteger(_lua, _key);
				lua_pushinteger(_lua, _scancode);
				lua_pushinteger(_lua, _action);
				lua_pushinteger(_lua, _mods);
				auto _err = lua_safecall(_lua, 4, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};
		};
		static void char_callback(GLFWwindow* _window, unsigned int _codepoint)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "text");

			if (lua_isfunction(_lua, -1))
			{
				lua_pushinteger(_lua, _codepoint);
				auto _err = lua_safecall(_lua, 1, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};
		};

		static void mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "mouse");

			if (lua_isfunction(_lua, -1))
			{
				lua_pushinteger(_lua, _button);
				lua_pushinteger(_lua, _action);
				lua_pushinteger(_lua, _mods);
				auto _err = lua_safecall(_lua, 3, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};
		};
		static void cursor_callback(GLFWwindow* _window, double _x, double _y)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "cursor");

			if (lua_isfunction(_lua, -1))
			{
				lua_pushnumber(_lua, _x);
				lua_pushnumber(_lua, _y);
				auto _err = lua_safecall(_lua, 2, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};

		};


		static void close_callback(GLFWwindow* _window)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "close");

			if (lua_isfunction(_lua, -1))
			{
				auto _err = lua_safecall(_lua, 0, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};
		};
		static void focus_callback(GLFWwindow* _window, int _focused)
		{
			auto _lua = ((window_data*)glfwGetWindowUserPointer(_window))->lua_;
			lua_getglobal(_lua, "SAEEngine");
			lua_getfield(_lua, -1, "window");
			lua_getfield(_lua, -1, "callback");
			lua_getfield(_lua, -1, "focus");

			if (lua_isfunction(_lua, -1))
			{
				lua_pushinteger(_lua, _focused);
				auto _err = lua_safecall(_lua, 1, 0, 0);
				lua_pop(_lua, 3);
			}
			else
			{
				lua_pop(_lua, 4);
			};
		};



		window_data(Window _window, lua_State* _lua) :
			window_{ std::move(_window) },
			lua_{ _lua }
		{

			glfwSetMouseButtonCallback(this->window_, &mouse_button_callback);
			glfwSetCursorPosCallback(this->window_, &cursor_callback);

			glfwSetKeyCallback(this->window_, &key_callback);
			glfwSetCharCallback(this->window_, &char_callback);


			glfwSetWindowCloseCallback(this->window_, &close_callback);
			glfwSetWindowFocusCallback(this->window_, &focus_callback);

		};

		Window window_;
		lua_State* lua_ = nullptr;



	};


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

	// TEMPORARY
	int window_update(lua_State* _lua)
	{
		auto _ptr = lua_getwindow(_lua);
		_ptr->window_.update();
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
		luaL_Reg{ "update", &window_update },
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

	int luaopen_window(lua_State* _lua)
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
#pragma endregion WINDOW_LIB

#pragma region OS_LIB
namespace sae::engine
{






	int os_sleep(lua_State* _lua)
	{
		auto _ms = lua_tointeger(_lua, -1);
		std::this_thread::sleep_for(std::chrono::milliseconds{ _ms });
		return 0;
	};

	const luaL_Reg engine_os_lib[] =
	{
		luaL_Reg{ "sleep", &os_sleep },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_os(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, engine_os_lib, 0);
		return 1;
	};


}
#pragma endregion OS_LIB

#pragma region IO_LIB
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
#pragma endregion IO_LIB

namespace sae::engine
{





	int SAEEngine::run_script(const std::filesystem::path& _path, std::nothrow_t)
	{
		auto _err = luaL_loadfile(this->lua(), _path.string().c_str());
		if (_err != LUA_OK)
			return _err;
		return lua_safecall(this->lua(), 0, LUA_MULTRET, 0, std::nothrow);
	};

#if SAE_ENGINE_USE_EXCEPTIONS
	int SAEEngine::run_script(const std::filesystem::path& _path)
	{
		auto _err = luaL_loadfile(this->lua(), _path.string().c_str());
		if (_err != LUA_OK)
		{
			throw std::runtime_error{ "luaL_loadfile()" };
			return _err;
		};
		return lua_safecall(this->lua(), 0, LUA_MULTRET, 0);
	};
#else
	int SAEEngine::run_script(const std::filesystem::path& _path)
	{
		return this->run_script(_path, std::nothrow);
	};
#endif

	bool SAEEngine::good() const
	{
		lua_getglobal(this->lua(), "SAEEngine");
		lua_getfield(this->lua(), -1, "window");
		auto _ptr = lua_towindow(this->lua(), -1, 1);
		lua_pop(this->lua(), 2);
		return _ptr->window_.good();
	};

	void SAEEngine::update()
	{
		auto _ptr = lua_getwindow(this->lua());
		_ptr->window_.update();
	};

	void SAEEngine::set_ostream(std::ostream* _ostr) noexcept
	{
		lua_pushstring(this->lua(), "SAEEngine.ostream");
		lua_pushlightuserdata(this->lua(), _ostr);
		lua_settable(this->lua(), LUA_REGISTRYINDEX);
	};
	std::ostream* SAEEngine::get_ostream() const
	{
		return lua_getostream(this->lua());
	};
	void SAEEngine::set_istream(std::istream* _istr) noexcept
	{
		lua_pushstring(this->lua(), "SAEEngine.istream");
		lua_pushlightuserdata(this->lua(), _istr);
		lua_settable(this->lua(), LUA_REGISTRYINDEX);
	};
	std::istream* SAEEngine::get_istream() const
	{
		return lua_getistream(this->lua());
	};

	SAEEngine::SAEEngine()
	{
		glfwInit();
		luaopen_SAEEngine(this->lua());
		lua_setglobal(this->lua(), "SAEEngine");
	};
	SAEEngine::~SAEEngine()
	{
		glfwTerminate();
	};


	int luaopen_SAEEngine(lua_State* _lua)
	{
		lua_newtable(_lua);

		luaopen_window(_lua);
		lua_setfield(_lua, -2, "window");
		luaopen_engine_io(_lua);
		lua_setfield(_lua, -2, "io");
		luaopen_engine_os(_lua);
		lua_setfield(_lua, -2, "os");

		return 1;
	};


}
