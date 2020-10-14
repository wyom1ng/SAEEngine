#pragma once

#include "SAEEngine.h"


namespace sae::engine
{

	std::ostream* lua_getostream(lua_State* _lua);
	std::istream* lua_getistream(lua_State* _lua);

	struct lib_io
	{
	private:

		// io.print(string)
		static int print(lua_State* _lua);

		// io.println(string)
		static int println(lua_State* _lua);

		// io.read(count) -> string
		static int read(lua_State* _lua);

		// io.readln() -> string
		static int readln(lua_State* _lua);

		constexpr static inline luaL_Reg funcs_f[] =
		{
			luaL_Reg{ "print", &print },
			luaL_Reg{ "println", &println },
			luaL_Reg{ "read", &read },
			luaL_Reg{ "readln", &readln },
			luaL_Reg{ NULL, NULL }
		};

	public:

		static int lua_open(lua_State* _lua);

	};

}