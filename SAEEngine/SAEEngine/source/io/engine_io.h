#pragma once

#include "SAEEngine.h"


namespace sae::engine
{

	std::ostream* lua_getostream(lua_State* _lua);
	std::istream* lua_getistream(lua_State* _lua);

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