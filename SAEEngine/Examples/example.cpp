
#include <SAEEngine.h>
#include <lua.hpp>

#include <cassert>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>

void test_function()
{
	sae::engine::SAEEngine _engine{};
	std::ofstream _logtestStream{ "C:\\Users\\jonat\\source\\repos\\SAEEngine\\SAEEngine\\Examples\\logtest.txt" };

	_engine.set_ostream(&std::cout);
	_engine.set_istream(&std::cin);

	auto _lua = _engine.lua();

	luaL_requiref(_lua, "package", &luaopen_package, false);
	
	try
	{
		auto _err = _engine.run_script("C:\\Users\\jonat\\source\\repos\\SAEEngine\\SAEEngine\\Examples\\example.lua");
	}
	catch (const std::runtime_error& _e)
	{
		std::cout << _e.what() << '\n';
		return;
	};

	while (_engine.good())
	{
		_engine.update();
	};

	_logtestStream.close();
}

int main()
{
	
	test_function();

	return 0;
}