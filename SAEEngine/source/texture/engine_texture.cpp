#include "engine_texture.h"

#include <cassert>

namespace sae::engine
{

	// texture.new(encoding, [width, height]) -> Texture_Data
	int lib_texture::new_f(lua_State* _lua)
	{
		auto _encoding = detail::encoding_from_string(lua_tostring(_lua, 1));
		size_t _width = 0;
		size_t _height = 0;

		if (lua_gettop(_lua) == 3)
		{
			_width = (size_t)lua_tointeger(_lua, 2);
			_height = (size_t)lua_tointeger(_lua, 3);
		};

		auto _ptr = lua::lua_newinstance<Texture>(_lua, tname(), _encoding, _width, _height);
		assert(_ptr);

		return 1;
	};

	// texture.new(encoding, path) -> Texture_Data
	int lib_texture::load_f(lua_State* _lua)
	{
		auto _encoding = detail::encoding_from_string(lua_tostring(_lua, 1));
		auto _path = lua_tostring(_lua, 2);
		
		auto _ptr = lua::lua_newinstance<Texture>(_lua, tname(), _encoding, _path);
		assert(_ptr);

		return 1;
	};



	// texture:width() -> integer
	int lib_texture::width(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		lua::lua_push(_lua, _ptr->width());
		return 1;
	};

	// texture:height() -> integer
	int lib_texture::height(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		lua::lua_push(_lua, _ptr->height());
		return 1;
	};

	// texture:size() -> integer
	int lib_texture::size(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		lua::lua_push(_lua, _ptr->size());
		return 1;
	};

	// texture:encoding() -> string
	int lib_texture::encoding(lua_State* _lua)
	{
		
		auto _ptr = to_texture(_lua, 1);
		auto _estr = detail::encoding_to_string(_ptr->encoding());
		if (!_estr.empty())
		{
			lua::lua_push(_lua, _estr.c_str());
		}
		else
		{
			lua::lua_push(_lua, lua::nil);
		};
		return 1;
	};

	// texture:stride() -> integer
	int lib_texture::stride(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		lua::lua_push(_lua, _ptr->stride());
		return 1;
	};

	// texture:bitdepth() -> integer
	int lib_texture::bitdepth(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		lua::lua_push(_lua, _ptr->bitdepth());
		return 1;
	};



	// texture:set_color(offset, count, color (see ENCODING))
	int lib_texture::set_color(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		auto _offset = (size_t)luaL_checkinteger(_lua, 2);
		auto _count = (size_t)luaL_checkinteger(_lua, 3);

		luaL_argcheck(_lua, lua_istable(_lua, 4), 4, "color value expected");

		std::vector<uint8_t> _cols{};
		auto n = lua::lua_getlen(_lua, 4);
		for (int i = 1; i <= n; ++i)
		{
			lua_rawgeti(_lua, 4, i);
			_cols.push_back((uint8_t)luaL_checkinteger(_lua, -1));
			lua_pop(_lua, 1);
		};

		auto _at = _ptr->view_begin() + _offset;
		_ptr->set_color(_at, _at + _count, _cols);

		return 0;
	};

	// texture:resize(width, height)
	int lib_texture::resize(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		size_t _w = (size_t)luaL_checkinteger(_lua, 2);
		size_t _h = (size_t)luaL_checkinteger(_lua, 3);
		_ptr->resize(_w, _h);
		return 0;
	};

	// texture:paste(Texture, x, y) -> bool
	int lib_texture::paste(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		auto _other = to_texture(_lua, 2);
		auto _x = (size_t)luaL_checkinteger(_lua, 3);
		auto _y = (size_t)luaL_checkinteger(_lua, 4);
		auto _result = _ptr->paste(*_other, _x, _y);
		lua_pushboolean(_lua, _result);
		return 1;
	};



	// texture:save(path)
	int lib_texture::save(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		auto _path = luaL_checkstring(_lua, 2);
		_ptr->save_to_file(_path);
		return 0;
	};


	// texture:__gc()
	int lib_texture::gc(lua_State* _lua)
	{
		auto _ptr = to_texture(_lua, 1);
		using value_type = std::remove_pointer_t<decltype(to_texture(std::declval<lua_State*>(), std::declval<int>()))>; // for shits and giggles
		_ptr->~value_type();
		return 0;
	};



	Texture* lib_texture::to_texture(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<Texture>(_lua, _idx, tname());
	};

	int lib_texture::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs_m, 0);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);

		return 1;
	};

}