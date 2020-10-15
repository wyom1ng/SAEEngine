#pragma once

#include "SAEEngine.h"

#include "gfx/engine_gfx.h"
#include "ui/engine_ui.h"

#include <vector>

namespace sae::engine
{

	
	
	struct Scene_Data
	{
	public:

		UIHandler* get_ui_handler();
		const UIHandler* get_ui_handler() const;

		void push_back(WidgetObject* _obj);

		void erase(WidgetObject* _obj);

		void clear() noexcept;

		void draw();
		void update();

		void activate() {};
		void deactivate() {};

		void set_display_size(int _width, int _height);

		~Scene_Data();

	private:
		UIHandler ui_{};
		std::vector<WidgetObject*> widgets_{};
		int window_width_ = 0;
		int window_height_ = 0;
	};



	struct lib_scene
	{
	public:

		struct ltype_scene
		{
		private:

			static int new_f(lua_State* _lua);

			static int push(lua_State* _lua);

			static int destructor(lua_State* _lua);

			constexpr static inline luaL_Reg funcs_f[] =
			{
				luaL_Reg{ "new", &new_f },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline luaL_Reg funcs_m[] =
			{
				luaL_Reg{ "push" , &push },
				luaL_Reg{ "__gc", &destructor },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.scene";

		public:

			constexpr static inline const char* tname() noexcept { return TYPENAME; };

			using value_type = Scene_Data;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			static int lua_open(lua_State* _lua);

		};



	private:

	public:

		static inline auto to_scene(lua_State* _lua, int _idx)
		{
			return ltype_scene::to_userdata(_lua, _idx);
		};

		static int lua_open(lua_State* _lua);

	};

}