#include "engine_os.h"

#include <chrono>
#include <thread>
#include <cassert>

namespace sae::engine
{

	int os_sleep(lua_State* _lua)
	{
		auto _ms = lua_tointeger(_lua, -1);
		std::this_thread::sleep_for(std::chrono::milliseconds{ _ms });
		return 0;
	};


	int luaopen_engine_os(lua_State* _lua)
	{
		lua_newtable(_lua);
		luaL_setfuncs(_lua, engine_os_lib, 0);
		return 1;
	};


}