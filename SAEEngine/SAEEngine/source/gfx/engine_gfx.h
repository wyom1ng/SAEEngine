#pragma once

#include "SAEEngine.h"

#include <glm/gtc/matrix_transform.hpp>

namespace sae::engine
{

	struct GFXObject
	{
		virtual void update() = 0;
		virtual ~GFXObject() = default;
	};

	struct WidgetObject : public GFXObject
	{
		void update() override {};
		void draw(const glm::mat4& _projectionMat);
		~WidgetObject() = default;
	};

	struct WorldObject : public GFXObject
	{
		void update() override {};
		void draw(const glm::mat4& _projectionMat, const glm::mat4& _cameraMat) {};
	};


	GFXObject* lua_toGFXObject(lua_State* _lua, int _idx, int _arg);

	int gfx_destructor(lua_State* _lua);

	int gfx_update(lua_State* _lua);
	
	int luaopen_engine_gfx(lua_State* _lua);



	int gfx_widgetobject_new(lua_State* _lua);

	int luaopen_engine_gfx_widget(lua_State* _lua);


	int gfx_worldobject_new(lua_State* _lua);

	int luaopen_engine_gfx_world(lua_State* _lua);



}