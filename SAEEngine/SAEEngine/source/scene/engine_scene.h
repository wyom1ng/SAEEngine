#pragma once

#include "SAEEngine.h"

#include "gfx/engine_gfx.h"


#include <vector>

namespace sae::engine
{
	struct Scene_Data
	{
	public:

		void push_back(WidgetObject* _obj);
		void erase(WidgetObject* _obj);

		void clear() noexcept;

		void draw();
		void update();

		void activate() {};
		void deactivate() {};

		~Scene_Data();
	private:
		std::vector<WidgetObject*> widgets_{};
	};

	Scene_Data* lua_toscenedata(lua_State* _lua, int _idx, int _arg);


	// engine.scene.new()
	int scene_new(lua_State* _lua);

	// engine.scene:push(GFXObject)
	int scene_push(lua_State* _lua);



	// engine.scene.__gc(scene)
	int scene_destructor(lua_State* _lua);



	int luaopen_engine_scene(lua_State* _lua);

}