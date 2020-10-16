#pragma once

#include "../../SAELib_Either.h"

#include <lua.hpp>

#include <new>
#include <functional>
#include <string_view>
#include <string>
#include <vector>

namespace sae::lua
{

	
	int lualib_tostring_f(lua_State* _lua);
	constexpr static inline luaL_Reg lualib_tostring{ "tostring", &lualib_tostring_f };



	/**
	 * @brief Creates a new class meta table associated with name _tname and leaves onto the top of the stack,
	 * if the name is already used, still pushes the table onto the top of the stack
	 * @param _lua lua state
	 * @param _tname name of the class
	 * @return 
	*/
	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions, int _upVals);
	void lua_newclass(lua_State* _lua, const char* _tname, const luaL_Reg* _functions);
	void lua_newclass(lua_State* _lua, const char* _tname);
	


	struct nil_t {};
	constexpr static inline nil_t nil{};
	

	template <typename T>
	void lua_push(lua_State* _lua, const T& _v);

	template <>
	static inline void lua_push(lua_State* _lua, const uint8_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const uint16_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const uint32_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const uint64_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};

	template <>
	static inline void lua_push(lua_State* _lua, const int8_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const int16_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const int32_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const int64_t& _v)
	{
		lua_pushinteger(_lua, (lua_Integer)_v);
	};

	template <>
	static inline void lua_push(lua_State* _lua, const float_t& _v)
	{
		lua_pushnumber(_lua, _v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const double_t& _v)
	{
		lua_pushnumber(_lua, _v);
	};
	template <>
	static inline void lua_push(lua_State* _lua, const long double& _v)
	{
		lua_pushnumber(_lua, _v);
	};

	template <>
	static inline void lua_push(lua_State* _lua, const char* const& _v)
	{
		lua_pushstring(_lua, _v);
	};

	template <>
	static inline void lua_push(lua_State* _lua, const nil_t& _v)
	{
		lua_pushnil(_lua);
	};






	int lua_inherit(lua_State* _lua, const char* _tname, int _idx);


	bool lua_isbaseof(lua_State* _lua, int _idx, const char* _baseName);


	void* lua_downcast(lua_State* _lua, int _idx, const char* _baseName);

	template <typename T> requires std::is_default_constructible<T>::value
		static inline T* lua_newudata(lua_State* _lua)
	{
		return new (lua_newuserdata(_lua, sizeof(T))) T{};
	};

	template <typename T, typename... CArgs>
	static inline T* lua_newudata(lua_State* _lua, CArgs... _arg)
	{
		return new (lua_newuserdata(_lua, sizeof(T))) T{ _arg... };
	};

	template <typename T> requires std::is_default_constructible<T>::value
		static inline T* lua_newinstance(lua_State* _lua, const char* _parentName)
	{
		auto _out = lua_newudata<T>(_lua);
		luaL_getmetatable(_lua, _parentName);
		lua_setmetatable(_lua, -2);
		return _out;
	};
	template <typename T, typename... CArgs>
	static inline T* lua_newinstance(lua_State* _lua, const char* _parentName, CArgs... _arg)
	{
		auto _out = lua_newudata<T, CArgs...>(_lua, _arg...);
		luaL_getmetatable(_lua, _parentName);
		lua_setmetatable(_lua, -2);
		return _out;
	};

	template <typename T>
	static inline T* lua_toinstance(lua_State* _lua, int _idx, const char* _tname)
	{
		void* ud = lua::lua_downcast(_lua, _idx, _tname);
		if (ud == nullptr)
		{
			luaL_typeerror(_lua, _idx, _tname);
		};
		return (T*)ud;
	};





	int lua_getlen(lua_State* _lua, int _idx);


	// Appends the element on the top of the stack to the table at _idx
	void lua_append(lua_State* _lua, int _idx);

	// Appends a copy of the element at _valueIdx to the end of the table at _tableIdx
	void lua_append_copy(lua_State* _lua, int _tableIdx, int _valueIdx);



	struct cfunction
	{
	public:
#ifdef lua_h
		using function_type = lua_CFunction;
#else
		using function_type = int(*)(lua_State* _lua);
#endif
		
		constexpr const char* name() const noexcept
		{
			return this->name_.data();
		};
		constexpr function_type func() const noexcept { return this->func_; };
		
		constexpr bool valid() const noexcept { return (this->func_ != nullptr) && (!this->name_.empty()); };
		constexpr explicit operator bool() const noexcept { return this->valid(); };

		int invoke(lua_State* _lua) const
		{
			return (*this->func_)(_lua);
		};
		int operator()(lua_State* _lua) const 
		{
			return this->invoke(_lua);
		};

		constexpr cfunction() noexcept :
			name_{}, func_{ nullptr }
		{};
		constexpr cfunction(std::string_view _name, function_type _func) noexcept :
			name_{ _name }, func_{ _func }
		{};

		constexpr cfunction(const cfunction& other) = default;
		cfunction& operator=(const cfunction& other) = default;

		constexpr cfunction(cfunction&& other) noexcept :
			name_{other.name_ },
			func_{ std::exchange(other.func_, nullptr) }
		{};
		cfunction& operator=(cfunction&& other) noexcept
		{
			std::swap(this->name_, other.name_);
			this->func_ = std::exchange(other.func_, nullptr);
			return *this;
		};

	private:
		std::string_view name_;
		function_type func_;
	};



	/*
		Food for thought
	 
	struct lua_ref
	{
	public:



		lua_ref(lua_State* _lua, int _i) :
			ref_{ luaL_ref(_lua, _i) }, 
			lua_{ _lua }, i_{ _i }
		{};
		~lua_ref()
		{
			luaL_unref(this->lua_, this->i_, this->ref_);
		};

	private:
		int i_ = 0;
		int ref_ = 0;
		lua_State* lua_ = nullptr;
	};
	*/

	

};