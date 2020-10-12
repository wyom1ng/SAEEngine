#include "LuaAPI.h"

#include "engine_gfx.h"

#include <new>
#include <iostream>


namespace sae::engine
{

	void WidgetObject::draw(const glm::mat4& _projectionMat)
	{

	};


	GFXObject* lua_toGFXObject(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.GFXObject");
		luaL_argcheck(_lua, ud != NULL, _arg, "`GFXObject' expected");
		return (GFXObject*)ud;
	};




	int gfx_destructor(lua_State* _lua)
	{
		auto _ptr = lua_toGFXObject(_lua, -1, 1);
		_ptr->~GFXObject();
		return 0;
	};

	int gfx_update(lua_State* _lua)
	{
		auto _ptr = lua_toGFXObject(_lua, -1, 1);
		_ptr->update();
		return 0;
	};

	

	const luaL_Reg gfx_lib[] =
	{
		luaL_Reg{ "__gc", &gfx_destructor },

		luaL_Reg{ NULL, NULL }
	};


	int luaopen_engine_gfx(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.GFXObject");
		lua_pushvalue(_lua, -1);
		lua_setfield(_lua, -2, "__index");

		luaL_setfuncs(_lua, gfx_lib, 0);

		luaopen_engine_gfx_widget(_lua);
		lua_setfield(_lua, -2, "widget");



		return 1;
	};





	int gfx_widgetobject_new(lua_State* _lua)
	{
		auto _ptr = lua::lua_newinstance<WidgetObject>(_lua, "SAEEngine.WidgetObject");
		return 1;
	};
	const luaL_Reg gfx_widget_lib[] =
	{
		luaL_Reg{ "new", &gfx_widgetobject_new },

		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_gfx_widget(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.WidgetObject");

		lua::lua_inherit(_lua, "SAEEngine.GFXObject", -1);

		luaL_setfuncs(_lua, gfx_widget_lib, 0);
		return 1;
	}


	int gfx_worldobject_new(lua_State* _lua)
	{

		return 1;
	};
	int luaopen_engine_gfx_world(lua_State* _lua)
	{

		return 1;
	};




}