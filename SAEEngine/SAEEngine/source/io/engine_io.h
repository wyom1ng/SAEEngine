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




	public:

		static int lua_open(lua_State* _lua);

	};

	// engine.io.print(string)
	int io_print(lua_State* _lua);

	// engine.io.println(string)
	int io_println(lua_State* _lua);

	// engine.io.read(count) -> string
	int io_read(lua_State* _lua);

	// engine.io.readln() -> string
	int io_readln(lua_State* _lua);

	int luaopen_engine_io(lua_State* _lua);

}