#pragma once

#include "SAEEngine.h"

namespace sae::engine
{
	// engine.os.sleep(milliseconds)
	int os_sleep(lua_State* _lua);

	int luaopen_engine_os(lua_State* _lua);

}