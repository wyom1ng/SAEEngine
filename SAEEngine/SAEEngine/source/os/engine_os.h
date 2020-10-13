#pragma once

#include "SAEEngine.h"

namespace sae::engine
{
	// engine.os.sleep(milliseconds)
	int os_sleep(lua_State* _lua);

	


	const luaL_Reg engine_os_lib[] =
	{
		luaL_Reg{ "sleep", &os_sleep },
		luaL_Reg{ NULL, NULL }
	};

	int luaopen_engine_os(lua_State* _lua);

}