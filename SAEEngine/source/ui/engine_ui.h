#pragma once
 
#include "SAEEngine.h"

#include "gfx/engine_gfx.h"

#include "SAELib_Either.h"
#include "SAELib_Functor.h"

#include <vector>

namespace sae::engine
{

	struct WidgetObject;



	struct UIButton
	{
		int invoke(lua_State* _lua, int _nargs) noexcept(noexcept(lua_safecall(nullptr, 0, 0, 0)));

		int widget_ref_;
		int callback_ref_;
		bool is_enabled_;
	};




	class UIManager
	{
	public:




	private:

	};





	struct lib_ui
	{
		struct ltype_button
		{
		private:

			// button.new(WidgetObject, callback, enabled) -> button
			// button.new(WidgetObject, callback) -> button
			static int new_f(lua_State* _lua);

			// button:invoke(args...)
			static int invoke(lua_State* _lua);

			// button:enable()
			static int enable(lua_State* _lua);

			// button:disable()
			static int disable(lua_State* _lua);

			// button:is_enabled() -> bool
			static int is_enabled(lua_State* _lua);

			// button:__gc()
			static int destructor(lua_State* _lua);

			constexpr static inline luaL_Reg funcs_f[] =
			{
				luaL_Reg{ "new", &new_f },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline luaL_Reg funcs_m[] =
			{
				luaL_Reg{ "invoke", &invoke },
				luaL_Reg{ "enable", &enable },
				luaL_Reg{ "disable", &disable },
				luaL_Reg{ "is_enabled", &is_enabled },
				luaL_Reg{ "__gc", &destructor },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.button";
		public:

			constexpr static inline const char* tname() noexcept { return TYPENAME; };

			using value_type = UIButton;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			static int lua_open(lua_State* _lua);

		};

	private:



	public:

		static inline auto to_button(lua_State* _lua, int _idx)
		{
			return ltype_button::to_userdata(_lua, _idx);
		};

		static int lua_open(lua_State* _lua);

	};

}