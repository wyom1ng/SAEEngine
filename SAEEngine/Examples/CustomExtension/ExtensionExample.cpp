#include <SAEEngine.h>
#include <SAEEngine_BaseTypes.h>

#include <iostream>

#include <chrono>
#include <thread>



struct example_Data : public sae::engine::WidgetObject
{
	bool good() const final
	{
		std::cout << "example_Data::good()\n";
		return true;
	};
	void update() final
	{
		std::cout << "example_Data::update()\n";
	};
	void draw(const glm::mat4& _projectionMat) final
	{
		if (!this->ran_draw_)
		{
			std::cout << "example_Data::draw()\n";
			this->ran_draw_ = true;
		};
	};
	void destroy() final
	{
		std::cout << "example_Data::destroy()\n";
	};

	void test()
	{
		std::cout << "ext.example.test() ran succesfully!\n";
	};

	~example_Data()
	{
		this->destroy();
		std::cout << "~example_Data()\n";
	};
	

	bool ran_draw_ = false;
};


example_Data* lua_toExample(lua_State* _lua, int _idx)
{
	void* ud = sae::lua::lua_downcast(_lua, _idx, "ExampleExt.Example_Data");
	luaL_argcheck(_lua, ud != NULL, _idx, "`Example_Data' expected");
	return (example_Data*)ud;
};

int example_new(lua_State* _lua)
{
	auto _new = sae::lua::lua_newinstance<example_Data>(_lua, "ExampleExt.Example_Data");
	return 1;
};

int example_test(lua_State* _lua)
{
	auto _ptr = lua_toExample(_lua, 1);
	_ptr->test();
	return 0;
};

luaL_Reg example_lib_f[] =
{
	luaL_Reg{ "new", &example_new },
	luaL_Reg{ "test", &example_test },
	luaL_Reg{ NULL, NULL }
};

int luaopen_testExtension(lua_State* _lua)
{
	sae::lua::lua_newclass(_lua, "ExampleExt.Example_Data");
	sae::lua::lua_inherit(_lua, "SAEEngine.WidgetObject", -1);
	luaL_setfuncs(_lua, example_lib_f, 0);
	return 1;
};


int main()
{
	sae::engine::SAEEngine _engine{};

	_engine.set_ostream(&std::cout);
	_engine.set_istream(&std::cin);

	sae::engine::Extension _exampleExt{ "example", &luaopen_testExtension };
	_engine.register_extension(_exampleExt);

	try
	{
		_engine.run_script("C:\\Users\\jonat\\Source\\Repos\\SAEEngine\\SAEEngine\\Examples\\CustomExtension\\usingExtensions.lua");
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