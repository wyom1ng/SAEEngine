#pragma once

#include "SAEEngineConfig.h"

#include <SAELua.h>
#include <lua.hpp>

#include <filesystem>
#include <unordered_map>

#if true
#define SAE_ENGINE_DESTRUCTOR_DEBUG
#endif

namespace std
{
	template<class CharT, class Traits = std::char_traits<CharT>>
	class basic_ostream;

	template<class CharT, class Traits = std::char_traits<CharT>>
	class basic_istream;

	using ostream = basic_ostream<char>;
	using istream = basic_istream<char>;

};

namespace sae::engine
{
	constexpr static inline auto SAE_ENGINE_GLFW_VERSION_MAJOR = 3;
	constexpr static inline auto SAE_ENGINE_GLFW_VERSION_MINOR = 3;

#ifdef SAE_ENGINE_OVERRIDE_GLFW_VERSION
	static_assert(GLFW_VERSION_MAJOR == SAE_ENGINE_GLFW_VERSION_MAJOR, "GLFW version mismatch! SAEEngine requires major version 3! Set SAE_ENGINE_OVERRIDE_GLFW_VERSION to true to override");
	static_assert(GLFW_VERSION_MINOR >= SAE_ENGINE_GLFW_VERSION_MINOR, "GLFW version mismatch! SAEEngine requires minor version >= 3! Set SAE_ENGINE_OVERRIDE_GLFW_VERSION to true to override");
#endif

	constexpr static inline auto SAE_ENGINE_LUA_VERSION = "Lua " "5" "." "4";
	
#ifdef SAE_ENGINE_OVERRIDE_LUA_VERSION 
	static_assert(SAE_ENGINE_LUA_VERSION == LUA_VERSION, "Lua version mismatch! SAEEngine requires lua version 5.4! Set SAE_ENGINE_OVERRIDE_LUA_VERSION to true to override");
#endif




	/**
	 * @brief noexcept version of lua_safecall()
	 * @param _lua lua state pointer, same as lua_pcall()
	 * @param _args number of lua function arguements, same as lua_pcall()
	 * @param _rets number of expected lua function returns, same as lua_pcall()
	 * @param _f same as lua_pcall()
	 * @param tag for no exception usage 
	 * @return same as lua_pcall()
	*/
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f, std::nothrow_t) noexcept;

	/**
	 * @brief wrapper for lua_pcall() with exception usage for unhandlable errors (only when SAE_ENGINE_USE_EXCEPTIONS is false)
	 * @param _lua lua state pointer, same as lua_pcall()
	 * @param _args number of lua function arguements, same as lua_pcall()
	 * @param _rets number of expected lua function returns, same as lua_pcall()
	 * @param _f same as lua_pcall()
	 * @exception std::runtime_error on lua_pcall() return value != LUA_OK
	 * @return LUA_OK on success, same as lua_pcall()
	*/
	int lua_safecall(lua_State* _lua, int _args, int _rets, int _f);


	struct Window;
	struct Lua
	{
	public:

		lua_State* get() const noexcept;

		operator lua_State*& () noexcept;
		operator lua_State* const& () const noexcept;

		bool good() const noexcept;
		explicit operator bool() const noexcept;

		void close();

		Lua();
		Lua(lua_Alloc _f, void* _ud);

		Lua(const Lua& other) = delete;
		Lua& operator=(const Lua& other) = delete;

		Lua(Lua&& other) noexcept;
		Lua& operator=(Lua&& other) noexcept;

		~Lua();

	private:
		lua_State* lua_ = nullptr;
	};

	struct window_data;





	struct Extension
	{
		std::string name;
		lua_CFunction open_function;
	};

	struct SAEEngine
	{
	public:
		lua_State* lua() const noexcept { return this->lua_; };

		bool good() const;
		void update();

		int run_script(const std::filesystem::path& _path, std::nothrow_t) noexcept;
#ifdef SAE_ENGINE_USE_EXCEPTIONS
		int run_script(const std::filesystem::path& _path);
#else 
		int run_script(const std::filesystem::path& _path) noexcept;
#endif


		void set_ostream(std::ostream* _ostr) noexcept;
		std::ostream* get_ostream() const;
		void set_istream(std::istream* _istr) noexcept;
		std::istream* get_istream() const;

		
		bool register_extension(const Extension& _ext);

		SAEEngine();
		~SAEEngine();

	private:
		Lua lua_;
	};


	

	int luaopen_SAEEngine(lua_State* _lua);



	






}
