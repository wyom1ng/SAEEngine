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

#if SAE_ENGINE_USE_EXCEPTIONS == ON
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





	int SAEEngine::run_script(const std::filesystem::path& _path, std::nothrow_t)
	{
		auto _err = luaL_loadfile(this->lua(), _path.string().c_str());
		if (_err != LUA_OK)
			return _err;
		return lua_safecall(this->lua(), 0, LUA_MULTRET, 0, std::nothrow);
	};

#if SAE_ENGINE_USE_EXCEPTIONS == ON
	int SAEEngine::run_script(const std::filesystem::path& _path)
	{
		auto _err = luaL_loadfile(this->lua(), _path.string().c_str());
		if (_err != LUA_OK)
		{
			throw std::runtime_error{ "luaL_loadfile()" };
			return _err;
		};
		return lua_safecall(this->lua(), 0, LUA_MULTRET, 0);
	};
#else
	int SAEEngine::run_script(const std::filesystem::path& _path)
	{
		return this->run_script(_path, std::nothrow);
	};
#endif

	bool SAEEngine::good() const
	{
		lua_getglobal(this->lua(), "SAEEngine");
		lua_getfield(this->lua(), -1, "window");
		auto _ptr = lua_towindow(this->lua(), -1, 1);
		lua_pop(this->lua(), 2);
		return _ptr->window_.good();
	};

	void SAEEngine::update()
	{
		auto _ptr = lua_getwindow(this->lua());
		_ptr->window_.update();
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
		luaopen_engine_io(_lua);
		lua_setfield(_lua, t, "io");
		luaopen_engine_os(_lua);
		lua_setfield(_lua, t, "os");
		luaopen_engine_scene(_lua);
		lua_setfield(_lua, t, "scene");

		return 1;
	};


}
