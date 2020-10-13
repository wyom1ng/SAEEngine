#pragma once

#include "SAEEngine.h"
#include "scene/engine_scene.h"
#include "shader/engine_shader.h"

#include <optional>
#include <exception>
#include <stdexcept>
#include <functional>
#include <stack>
#include <unordered_map>
#include <map>

namespace sae::engine
{

	struct Window
	{
	public:

		GLFWwindow* get() const noexcept;

		operator GLFWwindow*& () noexcept;
		operator GLFWwindow* const& () const noexcept;

		bool operator==(const Window& _other) const noexcept;
		bool operator!=(const Window& _other) const noexcept;

		bool operator==(GLFWwindow* _window) const noexcept;
		bool operator!=(GLFWwindow* _window) const noexcept;

		bool good() const noexcept;
		void close();

		explicit operator bool() const;


		void update();

		Window(int _width, int _height, const char* _title, std::optional<int> _monitor, std::optional<GLFWwindow*> _share);

		Window(int _width, int _height, const char* _title);

		Window(const Window& other) = delete;
		Window& operator=(const Window& other) = delete;

		Window(Window&& other) noexcept;
		Window& operator=(Window&& other) noexcept;

		~Window();

	private:
		GLFWwindow* window_ = nullptr;
	};
	struct window_data
	{

		static void key_callback(GLFWwindow* _window, int _key, int _scancode, int _action, int _mods);
		static void char_callback(GLFWwindow* _window, unsigned int _codepoint);

		static void mouse_button_callback(GLFWwindow* _window, int _button, int _action, int _mods);
		static void cursor_callback(GLFWwindow* _window, double _x, double _y);

		static void close_callback(GLFWwindow* _window);
		static void focus_callback(GLFWwindow* _window, int _focused);
		static void resize_callback(GLFWwindow* _window, int _width, int _height);

		Scene_Data* active_scene() const;
		
		void update();

		void push(Shader_Data* _shader);
		void erase(Shader_Data* _shader);




		window_data(Window _window, lua_State* _lua);
		~window_data();

		Window window_;
		lua_State* lua_ = nullptr;

		Shader_Data* active_shader_ = nullptr;
		std::unordered_map<std::string, Shader_Data*> shaders_{};
		
		bool is_valid_callback_key(const std::string& _key) const;
		void set_lua_callback(const std::string& _key, int _ref);
		void clear_lua_callback(const std::string& _key);
		const std::optional<int>& get_lua_callback(const std::string& _key) const;
		
	private:

		std::map<std::string, std::optional<int>> lua_callbacks_
		{
			{ "key", std::nullopt },
			{ "char", std::nullopt },

			{ "mouse", std::nullopt },
			{ "cursor", std::nullopt },

			{ "close", std::nullopt },
			{ "focus", std::nullopt },
			{ "resize", std::nullopt }

		};

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

	// window.push_shader(Shader_Data)
	int window_push_shader(lua_State* _lua);

	// window.erase_shader(Shader_Data)
	int window_erase_shader(lua_State* _lua);

	// window.get_shader(name) -> Shader_Data
	int window_get_shader(lua_State* _lua);

	// window.set_callback(name, function)
	int window_set_callback(lua_State* _lua);



	// window:__gc
	int window_destructor(lua_State* _lua);



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

		luaL_Reg{ "push_shader", &window_push_shader },
		luaL_Reg{ "erase_shader", &window_erase_shader },
		luaL_Reg{ "get_shader", &window_get_shader },

		luaL_Reg{ "set_callback", &window_set_callback },

		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_window(lua_State* _lua);




}