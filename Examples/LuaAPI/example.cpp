
#include <SAEEngine.h>
#include <lua.hpp>

#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

int main()
{
	sae::engine::SAEEngine _engine{};

	_engine.set_ostream(&std::cout);
	_engine.set_istream(&std::cin);

	auto _lua = _engine.lua();

	lua_register(_lua, sae::lua::lualib_tostring.name, sae::lua::lualib_tostring.func);

	
	try 
	{
		auto root = std::filesystem::path(PROJECT_ROOT);
		auto _err = _engine.run_script(root / "Examples/LuaAPI/engineStart.lua");
	}
	catch (const std::runtime_error& e)
	{
		std::cout << e.what() << '\n';
	};

	int _stackSize = 0;

	while (_engine.good())
	{
		assert(_stackSize == lua_gettop(_lua));
		_stackSize = lua_gettop(_lua);

		try
		{
			_engine.update();
		}
		catch (const std::runtime_error& e)
		{
			std::cout << e.what() << '\n';
		};
		std::this_thread::sleep_for(std::chrono::milliseconds{ 16 });
	};

	return 0;
};