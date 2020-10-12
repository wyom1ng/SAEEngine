#include "engine_scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include <new>
#include <algorithm>
#include <iostream>

namespace sae::engine
{

	Scene_Data::~Scene_Data() { std::cout << "~Scene_Data()\n"; };

	void Scene_Data::push_back(WidgetObject* _obj)
	{
		this->widgets_.push_back(_obj);
	};
	void Scene_Data::erase(WidgetObject* _obj)
	{
		this->widgets_.erase(std::find(this->widgets_.begin(), this->widgets_.end(), _obj));
	};

	void Scene_Data::clear() noexcept
	{
		this->widgets_.clear();
	};

	void Scene_Data::draw()
	{
		glm::mat4 _projectionMat = glm::ortho(0.0f, 800.0f, 0.0f, 600.0f);
		for (auto& w : this->widgets_)
			w->draw(_projectionMat);
	};
	void Scene_Data::update()
	{
		for (auto& w : this->widgets_)
			w->update();
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
			_ptr->push_back((WidgetObject*)lua::lua_downcast(_lua, 2, "SAEEngine.WidgetObject"));
		}
		else if (lua::lua_isbaseof(_lua, 2, "SAEEngine.WorldObject"))
		{
			abort();
		};
		return 0;
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
		luaL_Reg{ "push", &scene_push },
		luaL_Reg{ "__gc", &scene_destructor },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_scene(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.scene");
		luaL_setfuncs(_lua, scene_lib, 0);

		return 1;
	};








}