#include "engine_ui.h"

#include <numeric>
#include <iostream>

namespace sae::engine
{
	int UIButton::invoke(lua_State* _lua, int _nargs) noexcept(noexcept(lua_safecall(nullptr, 0, 0, 0)))
	{
		auto _beginTop = lua_gettop(_lua);

		auto _findex = (lua_gettop(_lua) - _nargs) + 1;

		lua_settop(_lua, lua_gettop(_lua) + 2);
		lua_rotate(_lua, _findex, 2);

		lua_rawgeti(_lua, LUA_REGISTRYINDEX, this->callback_ref_);
		lua_replace(_lua, _findex);

		lua_rawgeti(_lua, LUA_REGISTRYINDEX, this->ptr_->lua_self());
		lua_replace(_lua, _findex + 1);
	
		assert(lua_gettop(_lua) == (_nargs + 2));

		auto _res = lua_safecall(_lua, _nargs + 1, 0, 0);

		auto _endTop = lua_gettop(_lua);
		if (_res == LUA_ERRRUN)
			--_endTop;

		assert(_beginTop == (_endTop + _nargs));

		return _res;
	};

	UIButton::~UIButton()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~UIButton()\n";
#endif
	};


}

namespace sae::engine
{
	void UIHandler::push_back(UIButton* _b)
	{
		this->elements_.push_back(_b);
	};

	void UIHandler::erase(UIButton* _b)
	{
		this->elements_.erase(std::find(this->elements_.begin(), this->elements_.end(), _b));
	};

	void UIHandler::clear() noexcept
	{
		this->elements_.clear();
	};

	UIButton* UIHandler::find_best_match(int16_t _cursorX, int16_t _cursorY) const
	{
		std::vector<UIButton*> intersects_{};
		for (auto& e : this->elements_)
		{
			if (e->ptr_->intersects(_cursorX, _cursorY))
			{
				intersects_.push_back(e);
			};
		};

		if (!intersects_.empty())
		{
			auto _op = [](UIButton* _last, UIButton* _this)
			{
				return (_this->ptr_->get_position().z > _last->ptr_->get_position().z) ? _this : _last;
			};
			return std::accumulate(std::next(intersects_.begin()), intersects_.end(), intersects_.front(), _op);
		}
		else
		{
			return nullptr;
		};

	};

	UIHandler::~UIHandler()
	{
		this->clear();
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~UIHandler()\n";
#endif
	};

}

namespace sae::engine
{

	// button.new(WidgetObject, callback, enabled) -> button
	// button.new(WidgetObject, callback) -> button
	int lib_ui::ltype_button::new_f(lua_State* _lua)
	{
		auto _beginTop = lua_gettop(_lua);

		auto _widget = lib_gfx::to_widgetobject(_lua, 1);
		
		bool _enabled = true;
		if (lua_gettop(_lua) == 3)
		{
			_enabled = lua_toboolean(_lua, 3);
		};

		lua_pushvalue(_lua, 2);
		auto _ref = luaL_ref(_lua, LUA_REGISTRYINDEX);
		auto _ptr = lua::lua_newinstance<value_type>(_lua, tname(), _ref, _enabled, _widget);
		_ptr->ptr_ = _widget;
		
		lua_pushvalue(_lua, -1);
		_widget->get_decorators().push_back(luaL_ref(_lua, LUA_REGISTRYINDEX));

		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == (_endTop - 1));

		return 1;
	};



	// button:invoke(args...)
	int lib_ui::ltype_button::invoke(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->invoke(_lua, lua_gettop(_lua) - 1);
		return 0;
	};

	// button:enable()
	int lib_ui::ltype_button::enable(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->is_enabled_ = true;
		return 0;
	};

	// button:disable()
	int lib_ui::ltype_button::disable(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->is_enabled_ = false;
		return 0;
	};

	// button:is_enabled() -> bool
	int lib_ui::ltype_button::is_enabled(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		lua_pushboolean(_lua, _ptr->is_enabled_);
		return 1;
	};

	// button:__gc()
	int lib_ui::ltype_button::destructor(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		luaL_unref(_lua, LUA_REGISTRYINDEX, _ptr->callback_ref_);
		std::destroy_at(_ptr);
		return 0;
	};

	lib_ui::ltype_button::pointer lib_ui::ltype_button::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_ui::ltype_button::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs_m, 0);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);

		return 1;
	};


	int lib_ui::lua_open(lua_State* _lua)
	{
		lua_newtable(_lua);
		ltype_button::lua_open(_lua);
		lua_setfield(_lua, -2, "button");

		return 1;
	};

}