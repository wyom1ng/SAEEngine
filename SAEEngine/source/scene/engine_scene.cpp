#include "engine_scene.h"

#include <glm/gtc/matrix_transform.hpp>

#include <new>
#include <algorithm>
#include <iostream>

namespace sae::engine
{

	Scene_Data::~Scene_Data()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~Scene_Data()\n";
#endif
	};

	void Scene_Data::push_back(WidgetObject* _obj)
	{
		this->widgets_.push_back(_obj);
	};

	UIHandler* Scene_Data::get_ui_handler()
	{
		return &this->ui_;
	};
	const UIHandler* Scene_Data::get_ui_handler() const
	{
		return &this->ui_;
	};

	void Scene_Data::erase(WidgetObject* _obj)
	{
		this->widgets_.erase(std::find(this->widgets_.begin(), this->widgets_.end(), _obj));
	};

	void Scene_Data::clear() noexcept
	{
		this->widgets_.clear();
	};

	void Scene_Data::set_display_size(int _width, int _height)
	{
		this->window_width_ = _width;
		this->window_height_ = _height;
	};

	void Scene_Data::draw()
	{
		glm::mat4 _projectionMat = glm::ortho(0.0f, (float_t)this->window_width_, 0.0f, (float_t)this->window_height_);
		for (auto& w : this->widgets_)
			w->draw(_projectionMat);
	};
	void Scene_Data::update()
	{
		for (auto& w : this->widgets_)
			w->update();
	};

}

namespace sae::engine
{
	int lib_scene::ltype_scene::new_f(lua_State* _lua)
	{
		auto _ptr = lua::lua_newinstance<value_type>(_lua, tname());
		return 1;
	};

	int lib_scene::ltype_scene::push(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		if (lua::lua_isbaseof(_lua, 2, lib_gfx::ltype_WidgetObject::tname()))
		{
			_ptr->push_back(lib_gfx::to_widgetobject(_lua, 2));
		}
		else if (lua::lua_isbaseof(_lua, 2, lib_ui::ltype_button::tname()))
		{
			_ptr->get_ui_handler()->push_back(lib_ui::to_button(_lua, 2));
		}
		else if (lua::lua_isbaseof(_lua, 2, "SAEEngine.WorldObject"))
		{
			abort();
		};
		return 0;
	};

	int lib_scene::ltype_scene::destructor(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		std::destroy_at(_ptr);
		return 0;
	};

	lib_scene::ltype_scene::pointer lib_scene::ltype_scene::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_scene::ltype_scene::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs_m, 0);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);
		return 1;
	};




	int lib_scene::lua_open(lua_State* _lua)
	{
		ltype_scene::lua_open(_lua);
		return 1;
	};

}