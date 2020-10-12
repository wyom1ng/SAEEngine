#include "engine_scene.h"

#include <new>

namespace sae::engine
{

	struct Scene_Data
	{

		~Scene_Data() = default;
	};

	Scene_Data* lua_toscenedata(lua_State* _lua, int _idx, int _arg)
	{		
		void* ud = luaL_checkudata(_lua, _idx, "SAEEngine.scene");
		luaL_argcheck(_lua, ud != NULL, _arg, "`scene' expected");
		return (Scene_Data*)ud;
	};

	int scene_new(lua_State* _lua)
	{
		auto _ptr = new (lua_newuserdata(_lua, sizeof(Scene_Data))) Scene_Data{};
		luaL_getmetatable(_lua, "SAEEngine.scene");
		lua_setmetatable(_lua, -2);

		return 1;
	};







	int scene_destructor(lua_State* _lua)
	{
		auto _ptr = lua_toscenedata(_lua, -1, 1);
		_ptr->~Scene_Data();
		return 0;
	};



	const luaL_Reg scene_lib[] =
	{
		luaL_Reg{ "new", &scene_new },
		luaL_Reg{ "__gc", &scene_destructor },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_scene(lua_State* _lua)
	{
		luaL_newmetatable(_lua, "SAEEngine.scene");

		lua_pushvalue(_lua, -1);
		lua_setfield(_lua, -2, "__index");

		luaL_setfuncs(_lua, scene_lib, 0);

		lua_newtable(_lua);
		lua_setfield(_lua, -2, "objects");


		return 1;
	};








}