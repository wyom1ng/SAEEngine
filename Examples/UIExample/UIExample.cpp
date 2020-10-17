#include <SAEEngine.h>

#include <iostream>
#include <chrono>
#include <thread>




int main()
{
	sae::engine::SAEEngine _engine{};

	_engine.set_ostream(&std::cout);
	_engine.set_istream(&std::cin);

	luaL_openlibs(_engine.lua());
	
	try
	{
		auto root = std::filesystem::path(PROJECT_ROOT);
		_engine.run_script(root / "Examples/UIExample/engineStart.lua");
	}
	catch (const std::runtime_error& _e)
	{
		std::cout << _e.what() << '\n';
	};

	while (_engine.good())
	{
		_engine.update();
	};

	return 0;
};