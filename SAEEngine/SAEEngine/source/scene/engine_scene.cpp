#include "engine_scene.h"

#include <new>
#include <vector>

namespace sae::engine
{

	struct Scene_Data
	{
	public:

		void push_back(WidgetObject* _obj)
		{
			this->widgets_.push_back(_obj);
		};


		void draw()
		{

		};
		void update()
		{

		};


		~Scene_Data() = default;
	private:
		std::vector<WidgetObject*> widgets_{};
	};

	Scene_Data* lua_toscenedata(lua_State* _lua, int _idx, int _arg)
	{		
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.scene");
		luaL_argcheck(_lua, ud != NULL, _arg, "`scene' expected");
		return (Scene_Data*)ud;
	};

	int scene_new(lua_State* _lua)
	{
		auto _ptr = lua::lua_newinstance<Scene_Data>(_lua, "SAEEngine.scene");
		return 1;
	};

	int scene_push(lua_State* _lua)
	{
		auto _ptr = lua_toscenedata(_lua, 1, 1);
		if (lua::lua_isbaseof(_lua, 2, "SAEEngine.WidgetObject"))
		{

		};


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


		return 1;
	};








}