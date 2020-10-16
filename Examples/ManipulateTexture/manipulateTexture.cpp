#include <SAEEngine.h>

#include <iostream>

int main()
{
	sae::engine::SAEEngine _engine{};

	_engine.set_ostream(&std::cout);

	auto _lua = _engine.lua();
	lua_register(_lua, sae::lua::lualib_tostring.name, sae::lua::lualib_tostring.func);

	try
	{
		auto root = std::filesystem::path(PROJECT_ROOT);
		_engine.run_script(root / "Examples/ManipulateTexture/manip.lua");
	}
	catch (std::runtime_error _e)
	{
		std::cout << _e.what() << '\n';
	};

	while (_engine.good())
	{
		try
		{
			_engine.update();
		}
		catch (std::runtime_error _e)
		{
			std::cout << _e.what() << '\n';
		};
	};

	return 0;
}