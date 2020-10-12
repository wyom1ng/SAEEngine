#pragma once

#include "SAEEngine.h"

#include "io/engine_io.h"
#include "os/engine_os.h"
#include "window/engine_window.h"
#include "scene/engine_scene.h"

#include <optional>

namespace sae::engine
{










	struct Scene_Data;
	Scene_Data* lua_toscenedata(lua_State* _lua, int _idx, int _arg);

	int luaopen_engine_scene(lua_State* _lua);


}