#include <SAEEngine.h>

#include <iostream>
#include <chrono>
#include <thread>




int main()
{
	sae::engine::SAEEngine _engine{};

	_engine.set_ostream(&std::cout);
	_engine.set_istream(&std::cin);

	try
	{
		_engine.run_script("C:\\Users\\jonat\\Source\\Repos\\SAEEngine\\Examples\\UIExample\\createUIElement.lua");
	}
	catch (const std::runtime_error& _e)
	{
		std::cout << _e.what() << '\n';
	};

	while (_engine.good())
	{
		_engine.update();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
	};

	return 0;
};