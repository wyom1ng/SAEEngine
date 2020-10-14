#include "SAEEngine.h"
#include "LuaAPI.h"

#include <functional>
#include <stdexcept>

namespace sae::engine
{
	lua_State* Lua::get() const noexcept { return this->lua_; };

	Lua::operator lua_State*& () noexcept { return this->lua_; };
	Lua::operator lua_State* const& () const noexcept { return this->lua_; };

	bool Lua::good() const noexcept { return this->lua_ != nullptr; };
	Lua::operator bool() const noexcept { return this->good(); };

	void Lua::close()
	{
		if (this->good())
		{
			lua_close(this->lua_);
			this->lua_ = nullptr;
		};
	};

	Lua::Lua() :
		lua_{ luaL_newstate() }
	{};
	Lua::Lua(lua_Alloc _f, void* _ud) :
		lua_{ lua_newstate(_f, _ud) }
	{};

	Lua::Lua(Lua&& other) noexcept :
		lua_{ std::exchange(other.lua_, nullptr) }
	{};
	Lua& Lua::operator=(Lua&& other) noexcept
	{
		this->lua_ = std::exchange(other.lua_, nullptr);
		return *this;
	};

	Lua::~Lua()
	{
		this->close();
	}
};

namespace sae::engine
{

	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f, std::nothrow_t) noexcept
	{
		return lua_pcall(_lua, _args, _rets, _f);
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f)
	{
		auto _err = lua_pcall(_lua, _args, _rets, _f);
		switch (_err)
		{
		case LUA_OK:
			break;
		case LUA_ERRRUN:
			throw std::runtime_error{ std::string{lua_tostring(_lua, -1)} };
		default:
			throw std::runtime_error{ "lua_pcall() error" };
		};
		return _err;
	};
#else
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f) noexcept
	{
		return lua_safecall(_lua, _args, _rets, _f, std::nothrow);
	};
#endif

};

namespace sae::engine
{

	int SAEEngine::run_script(const std::filesystem::path& _path, std::nothrow_t) noexcept
	{
		auto _beginTop = lua_gettop(this->lua());
		
		lua_pushstring(this->lua(), _path.string().c_str());
		auto _out = fs_dofile(this->lua(), std::nothrow);
		lua_pop(this->lua(), 1);

		auto _endTop = lua_gettop(this->lua());
		assert(_beginTop == _endTop);

		return _out;
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	int SAEEngine::run_script(const std::filesystem::path& _path)
	{
		auto _beginTop = lua_gettop(this->lua());

		lua_pushstring(this->lua(), _path.string().c_str());
		auto _out = fs_dofile(this->lua());
		lua_pop(this->lua(), 1);

		auto _endTop = lua_gettop(this->lua());
		assert(_beginTop == _endTop);

		return _out;
	};
#else
	int SAEEngine::run_script(const std::filesystem::path& _path) noexcept
	{
		return this->run_script(_path, std::nothrow);
	};
#endif



	bool SAEEngine::good() const
	{
		auto _beginTop = lua_gettop(this->lua());

		assert(lua_getglobal(this->lua(), "SAEEngine") == LUA_TTABLE);
		lua_getfield(this->lua(), -1, "window");
		auto _ptr = lua_towindow(this->lua(), -1, 1);
		lua_pop(this->lua(), 2);

		auto _endTop = lua_gettop(this->lua());
		assert(_beginTop == _endTop);

		return _ptr->window_.good();
	};

	void SAEEngine::update()
	{
		auto _lua = this->lua();
		auto _beginTop = lua_gettop(_lua);

		lua_pushstring(_lua, NEXT_SCRIPT_KEY);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		if (!lua_isnoneornil(this->lua(), -1))
		{
			std::filesystem::path _path = lua_tostring(_lua, -1);
			lua_pop(_lua, 1);
			lua_pushstring(_lua, NEXT_SCRIPT_KEY);
			lua_pushnil(_lua);
			lua_settable(_lua, LUA_REGISTRYINDEX);
			this->run_script(_path);
		}
		else
		{
			lua_pop(_lua, 1);
		};

		auto _ptr = lua_getwindow(_lua);
		_ptr->update();

		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == _endTop);
	};

	void SAEEngine::set_ostream(std::ostream* _ostr) noexcept
	{
		lua_pushstring(this->lua(), "SAEEngine.ostream");
		lua_pushlightuserdata(this->lua(), _ostr);
		lua_settable(this->lua(), LUA_REGISTRYINDEX);
	};
	std::ostream* SAEEngine::get_ostream() const
	{
		return lua_getostream(this->lua());
	};
	void SAEEngine::set_istream(std::istream* _istr) noexcept
	{
		lua_pushstring(this->lua(), "SAEEngine.istream");
		lua_pushlightuserdata(this->lua(), _istr);
		lua_settable(this->lua(), LUA_REGISTRYINDEX);
	};
	std::istream* SAEEngine::get_istream() const
	{
		return lua_getistream(this->lua());
	};


	constexpr auto EXTENSIONS_KEY = "SAEEngine.ext";



	bool SAEEngine::register_extension(const Extension& _ext)
	{
		auto _lua = this->lua();
		auto _beginTop = lua_gettop(_lua);
		
		bool _out = false;

		lua_pushstring(_lua, EXTENSIONS_KEY);
		lua_gettable(_lua, LUA_REGISTRYINDEX);
		assert(lua_istable(_lua, -1));

		lua_getfield(_lua, -1, _ext.name.c_str());
		if (lua_isnil(_lua, -1))
		{
			lua_pop(_lua, 1);
			
			auto _beforeOpen = lua_gettop(_lua);
			auto _ret = _ext.open_function(_lua);
			auto _afterOpen = lua_gettop(_lua);
			
			assert(_ret == 1);
			assert(_beforeOpen == (_afterOpen - _ret));
			assert(lua_istable(_lua, -1));

			lua_setfield(_lua, _beforeOpen, _ext.name.c_str());

			_out = true;
		}
		else
		{
			lua_pop(_lua, 1);
		};

		lua_pop(_lua, 1);

		auto _endTop = lua_gettop(_lua);
		assert(_beginTop == _endTop);

		return _out;
	};


	SAEEngine::SAEEngine()
	{
		glfwInit();
		luaopen_SAEEngine(this->lua());
		lua_setglobal(this->lua(), "SAEEngine");
	};
	SAEEngine::~SAEEngine()
	{
		glfwTerminate();
	};


	int luaopen_SAEEngine(lua_State* _lua)
	{
		lua_newtable(_lua);
		auto t = lua_gettop(_lua);

		luaopen_engine_window(_lua);
		lua_setfield(_lua, t, "window");

		lib_io::lua_open(_lua);
		lua_setfield(_lua, t, "io");

		luaopen_engine_os(_lua);
		lua_setfield(_lua, t, "os");

		luaopen_engine_scene(_lua);
		lua_setfield(_lua, t, "scene");

		luaopen_engine_gfx(_lua);
		lua_setfield(_lua, t, "gfx");
	
		luaopen_engine_shader(_lua);
		lua_setfield(_lua, t, "shader");

		luaopen_engine_fs(_lua);
		lua_setfield(_lua, t, "fs");

		lib_texture::lua_open(_lua);
		lua_setfield(_lua, t, "texture");



		lua_newtable(_lua);
		lua_pushstring(_lua, EXTENSIONS_KEY);
		lua_pushvalue(_lua, -2);
		lua_settable(_lua, LUA_REGISTRYINDEX);
		lua_setfield(_lua, t, "ext");


		return 1;
	};


}
