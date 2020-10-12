#pragma once

#include "SAEEngine.h"
#include "scene/engine_scene.h"

#include <optional>
#include <exception>
#include <stdexcept>
#include <functional>
#include <stack>

namespace sae::engine
{

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

		Scene_Data* active_scene() const;
		
		void update();


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



	window_data* lua_towindow(lua_State* _lua, int _idx, int _arg);
	window_data* lua_getwindow(lua_State* _lua);

	// window.open(width, height, title, [monitor])
	int window_open(lua_State* _lua);

	// window.close()
	int window_close(lua_State* _lua);

	// window.get_size() -> width, height
	int window_get_size(lua_State* _lua);

	// window.set_size(width, height)
	int window_set_size(lua_State* _lua);

	// window.iconify()
	int window_iconify(lua_State* _lua);

	// window.attention()
	int window_attention(lua_State* _lua);

	// window.hide()
	int window_hide(lua_State* _lua);

	// window.show()
	int window_show(lua_State* _lua);

	// window.focus()
	int window_focus(lua_State* _lua);

	// window.push_scene(Scene_Data)
	int window_push_scene(lua_State* _lua);

	// window.pop_scene()
	int window_pop_scene(lua_State* _lua);

	// window.scene_stack_size() -> integer
	int window_scene_stack_size(lua_State* _lua);

	// window.has_active_scene() -> bool
	int window_has_active_scene(lua_State* _lua);



	// window:__gc
	int window_destructor(lua_State* _lua);


	int luaopen_engine_window(lua_State* _lua);




}