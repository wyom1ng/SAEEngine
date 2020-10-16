#include "LuaAPI.h"

#include "engine_gfx.h"

#include <new>
#include <iostream>
#include <stdexcept>
#include <string>

#define glassert assert(glGetError() == GL_NO_ERROR)

namespace sae::engine
{
	bool vertex_array_object::good() const noexcept
	{
		return (this->id != 0);
	};
	vertex_array_object::operator bool() const noexcept
	{
		return this->good();
	};

	void vertex_array_object::bind()
	{
		glBindVertexArray(this->id);
	};
	void vertex_array_object::unbind()
	{
		glBindVertexArray(0);
	};

	void vertex_array_object::destroy()
	{
		if (this->good())
		{
			glDeleteVertexArrays(1, &this->id);
			this->id = 0;
		};
	};

	vertex_array_object::vertex_array_object()
	{
		glGenVertexArrays(1, &this->id);
	};
	vertex_array_object::~vertex_array_object()
	{
		this->destroy();
	};

}


namespace sae::engine
{
	bool vertex_buffer_object::good() const noexcept
	{
		return (this->id != 0);
	};
	vertex_buffer_object::operator bool() const noexcept
	{
		return this->good();
	};

	void vertex_buffer_object::bind(GLenum _target)
	{
		glBindBuffer(_target, this->id);
	};
	void vertex_buffer_object::unbind(GLenum _target)
	{
		glBindBuffer(_target, 0);
	};


	void vertex_buffer_object::destroy()
	{
		if (this->good())
		{
			glDeleteBuffers(1, &this->id);
			this->id = 0;
		};
	};

	vertex_buffer_object::vertex_buffer_object()
	{
		glGenBuffers(1, &this->id);
	};
	vertex_buffer_object::~vertex_buffer_object()
	{
		this->destroy();
	};


}


namespace sae::engine
{

	void vertex_attribute::enable() const
	{
		glEnableVertexAttribArray(this->index);
	};
	void vertex_attribute::disable() const
	{
		glDisableVertexAttribArray(this->index);
	};
	void vertex_attribute::set() const
	{
		glVertexAttribPointer(this->index, this->size, this->type, this->normalize, this->stride, (void*)this->offset);
	};

	vertex_attribute::vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize, uint16_t _stride, uint16_t _offset) :
		index{ _index }, size{ _size }, type{ _type }, normalize{ _normalize }, stride{ _stride }, offset{ _offset }
	{};
	vertex_attribute::vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize, uint16_t _stride) : 
		vertex_attribute{ _index, _size, _type, _normalize, _stride, 0 }
	{};
	vertex_attribute::vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize) :
		vertex_attribute{ _index, _size, _type, _normalize, 0 }
	{};
	vertex_attribute::vertex_attribute(GLuint _index, uint8_t _size, GLenum _type) :
		vertex_attribute{ _index, _size, _type, false }
	{};

}

namespace sae::engine
{

	void WidgetObject::set_color(color_rgba _col) noexcept
	{
		this->color_ = _col;
		this->update();
	};
	color_rgba WidgetObject::get_color() const noexcept
	{
		return this->color_;
	};

	void WidgetObject::set_position(position2D _pos) noexcept
	{
		this->position_ = _pos;
		this->update();
	};
	position2D WidgetObject::get_position() const noexcept
	{
		return this->position_;
	};

	void WidgetObject::set_size(size2D _s) noexcept
	{
		this->size_ = _s;
		this->update();
	};
	size2D WidgetObject::get_size() const noexcept
	{
		return this->size_;
	};



	bool WidgetObject::intersects(int16_t _x, int16_t _y) const noexcept
	{
		return	((this->position_.x <= _x) && (_x < (this->position_.x + this->size_.width)) &&
				 (this->position_.y <= _y) && (_y < (this->position_.y + this->size_.height)));
	};



	bool Widget_Rectangle::good() const
	{
		return this->vao_.good();
	};
	void Widget_Rectangle::update()
	{
		auto _c = this->get_color();
		for (auto p = this->cols_.data(); p < (this->cols_.data() + this->cols_.size()); p += 4)
		{
			*(p + 0) = _c.r;
			*(p + 1) = _c.g;
			*(p + 2) = _c.b;
			*(p + 3) = _c.a;
		};
		this->cols_.update(0, this->cols_.size());

		auto _pos = this->get_position();
		auto _size = this->get_size();
		auto p = this->pos_.data();

		p[0] = _pos.x;
		p[1] = _pos.y;
		p[2] = _pos.z;

		p[3] = _pos.x;
		p[4] = _pos.y + _size.height;
		p[5] = _pos.z;

		p[6] = _pos.x + _size.width;
		p[7] = _pos.y;
		p[8] = _pos.z;

		p[9] = _pos.x + _size.width;
		p[10] = _pos.y + _size.height;
		p[11] = _pos.z;

		this->pos_.update(0, this->pos_.size());

	};
	void Widget_Rectangle::draw(const glm::mat4& _projectionMat)
	{
		this->shader_->bind();
		this->vao_.bind();
		glUniformMatrix4fv(this->projection_uniform_, 1, GL_FALSE, &_projectionMat[0][0]);
		this->indices_.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
		this->vao_.unbind();
	};
	void Widget_Rectangle::destroy()
	{
		this->vao_.destroy();
		this->pos_.destroy();
		this->cols_.destroy();
		this->indices_.destroy();
	};

	Widget_Rectangle::Widget_Rectangle(Shader_Data* _shader) :
		shader_{ _shader }
	{
#ifdef SAE_ENGINE_HARD_ERRORS
		assert(this->shader_ != nullptr);
#else
#ifdef SAE_ENGINE_USE_EXCEPTIONS
		throw std::runtime_error{ "no shader was set" };
#else
		this->vao_ = 0;
		return;
#endif
#endif
		assert(!glGetError());

		this->projection_uniform_ = glGetUniformLocation(this->shader_->id(), "ProjectionMatrix");
		this->vao_.bind();

		assert(this->vao_.good());
		assert(this->pos_.good());
		assert(this->cols_.good());
		assert(this->indices_.good());

		this->pos_attr_.enable();
		this->pos_.bind();
		this->pos_attr_.set();
		this->pos_.realloc_buffer();
		assert(!glGetError());

		this->col_attr_.enable();
		this->cols_.bind();
		this->col_attr_.set();
		this->cols_.realloc_buffer();
		assert(!glGetError());

		this->indices_.bind();
		this->indices_.realloc_buffer();

		auto _glError = glGetError();
		if (_glError != GL_NO_ERROR)
		{
#ifdef SAE_ENGINE_HARD_ERRORS
			abort();
#ifdef SAE_ENGINE_USE_EXCEPTIONS
			throw std::runtime_error{ "OpenGL error : " + std::to_string(_glError) };
#else
			this->vao_ = 0;
#endif
#endif
		};

		this->vao_.unbind();
	};
	Widget_Rectangle::~Widget_Rectangle()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~Widget_Rectangle()\n";
#endif
	};



}

namespace sae::engine
{

	bool Widget_Sprite::good() const
	{
		return this->vao_.good();
	};
	void Widget_Sprite::update()
	{
		auto _c = this->get_color();
		for (auto p = this->cols_.data(); p < (this->cols_.data() + this->cols_.size()); p += 4)
		{
			*(p + 0) = _c.r;
			*(p + 1) = _c.g;
			*(p + 2) = _c.b;
			*(p + 3) = _c.a;
		};
		this->cols_.update(0, this->cols_.size());

		auto _pos = this->get_position();
		auto _size = this->get_size();
		auto p = this->pos_.data();

		p[0] = _pos.x;
		p[1] = _pos.y;
		p[2] = _pos.z;

		p[3] = _pos.x;
		p[4] = _pos.y + _size.height;
		p[5] = _pos.z;

		p[6] = _pos.x + _size.width;
		p[7] = _pos.y;
		p[8] = _pos.z;

		p[9] = _pos.x + _size.width;
		p[10] = _pos.y + _size.height;
		p[11] = _pos.z;

		this->pos_.update(0, this->pos_.size());
	};
	void Widget_Sprite::draw(const glm::mat4& _projectionMat)
	{
		this->shader_->bind();
		glBindTexture(GL_TEXTURE_2D, texture_id_);

		this->vao_.bind();
		glUniformMatrix4fv(this->projection_uniform_, 1, GL_FALSE, &_projectionMat[0][0]);
		this->indices_.bind();
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, NULL);
		this->vao_.unbind();

	};
	void Widget_Sprite::destroy()
	{
		glDeleteTextures(1, &this->texture_id_);
	};

	Widget_Sprite::Widget_Sprite(Shader_Data* _shader, Texture* _tex) : 
		shader_{ _shader }, image_{ _tex }
	{

		assert((bool)_tex);

#ifdef SAE_ENGINE_HARD_ERRORS
		assert(this->shader_ != nullptr);
#else
#ifdef SAE_ENGINE_USE_EXCEPTIONS
		throw std::runtime_error{ "no shader was set" };
#else
		this->vao_ = 0;
		return;
#endif
#endif

		assert(!glGetError());

		

		glGenTextures(1, &this->texture_id_);
		glBindTexture(GL_TEXTURE_2D, this->texture_id_);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, this->image_->width(), this->image_->height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, this->image_->data());

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);



		this->projection_uniform_ = glGetUniformLocation(this->shader_->id(), "ProjectionMatrix");
		this->vao_.bind();

		assert(this->vao_.good());
		assert(this->pos_.good());
		assert(this->cols_.good());
		assert(this->indices_.good());

		this->pos_attr_.enable();
		this->pos_.bind();
		this->pos_attr_.set();
		this->pos_.realloc_buffer();
		assert(!glGetError());

		this->col_attr_.enable();
		this->cols_.bind();
		this->col_attr_.set();
		this->cols_.realloc_buffer();
		assert(!glGetError());

		this->uv_attr_.enable();
		this->uvs_.bind();
		this->uv_attr_.set();
		this->uvs_.realloc_buffer();
		assert(!glGetError());

		this->indices_.bind();
		this->indices_.realloc_buffer();

		auto _glError = glGetError();
		if (_glError != GL_NO_ERROR)
		{
#ifdef SAE_ENGINE_HARD_ERRORS
			abort();
#ifdef SAE_ENGINE_USE_EXCEPTIONS
			throw std::runtime_error{ "OpenGL error : " + std::to_string(_glError) };
#else
			this->vao_ = 0;
#endif
#endif
		};

		this->vao_.unbind();



	};
	Widget_Sprite::~Widget_Sprite()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~Widget_Sprite()\n";
#endif
	};

}



namespace sae::engine
{


	GFXObject* lua_toGFXObject(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.GFXObject");
		luaL_argcheck(_lua, ud != NULL, _arg, "`GFXObject' expected");
		return (GFXObject*)ud;
	};








	WidgetObject* lua_towidget(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.WidgetObject");
		luaL_argcheck(_lua, ud != NULL, _arg, "`WidgetObject' expected");
		return (WidgetObject*)ud;
	};


	Widget_Rectangle* lua_torectangle(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.Rectangle");
		luaL_argcheck(_lua, ud != NULL, _arg, "'Rectangle' expected");
		return (Widget_Rectangle*)ud;
	};

	// gfx.rectangle.new(Shader_Data) -> rectangle
	int gfx_rectangle_new(lua_State* _lua)
	{
		auto _shader = lib_shader::to_shader(_lua, 1);
		auto _ptr = lua::lua_newinstance<Widget_Rectangle>(_lua, "SAEEngine.Rectangle", _shader);
		return 1;
	};

	int luaopen_engine_gfx_rectangle(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.Rectangle");
		lua::lua_inherit(_lua, "SAEEngine.WidgetObject", -1);
		luaL_setfuncs(_lua, gfx_rectangle_lib, 0);
		return 1;
	};

}


namespace sae::engine
{

	int lib_gfx::ltype_GFXObject::good(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		lua_pushboolean(_lua, _ptr->good());
		return 1;
	};
	int lib_gfx::ltype_GFXObject::update(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->update();
		return 0;
	};
	int lib_gfx::ltype_GFXObject::destructor(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		for (auto& d : _ptr->get_decorators())
			luaL_unref(_lua, LUA_REGISTRYINDEX, d);
		_ptr->~GFXObject();
		return 0;
	};

	lib_gfx::ltype_GFXObject::pointer lib_gfx::ltype_GFXObject::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_gfx::ltype_GFXObject::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs, 0);
		lua_pop(_lua, 1);
		return 0;
	};

}


namespace sae::engine
{

	int lib_gfx::ltype_WidgetObject::set_color(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);

		color_rgba _col{};

		lua_getfield(_lua, 2, "r");
		_col.r = (uint8_t)lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "g");
		_col.g = (uint8_t)lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "b");
		_col.b = (uint8_t)lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "a");
		_col.a = (uint8_t)lua_tointeger(_lua, -1);

		lua_pop(_lua, 4);
		_ptr->set_color(_col);

		return 0;
	};

	int lib_gfx::ltype_WidgetObject::get_color(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		auto _col = _ptr->get_color();

		lua_newtable(_lua);
		lua_pushinteger(_lua, (lua_Integer)_col.r);
		lua_setfield(_lua, -2, "r");
		lua_pushinteger(_lua, (lua_Integer)_col.g);
		lua_setfield(_lua, -2, "g");
		lua_pushinteger(_lua, (lua_Integer)_col.b);
		lua_setfield(_lua, -2, "b");
		lua_pushinteger(_lua, (lua_Integer)_col.a);
		lua_setfield(_lua, -2, "a");

		return 1;
	};

	int lib_gfx::ltype_WidgetObject::set_position(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		auto _pos = position2D{};

		lua_getfield(_lua, 2, "x");
		_pos.x = lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "y");
		_pos.y = lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "z");
		_pos.z = lua_tonumber(_lua, -1);

		lua_pop(_lua, 3);
		_ptr->set_position(_pos);

		return 0;
	};

	int lib_gfx::ltype_WidgetObject::get_position(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		auto _p = _ptr->get_position();

		lua_newtable(_lua);
		lua_pushinteger(_lua, _p.x);
		lua_setfield(_lua, -2, "x");
		lua_pushinteger(_lua, _p.y);
		lua_setfield(_lua, -2, "y");
		lua_pushnumber(_lua, _p.z);
		lua_setfield(_lua, -2, "z");

		return 1;
	};

	int lib_gfx::ltype_WidgetObject::set_size(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		auto _s = size2D{};

		lua_getfield(_lua, 2, "w");
		_s.width = lua_tointeger(_lua, -1);

		lua_getfield(_lua, 2, "h");
		_s.height = lua_tointeger(_lua, -1);

		lua_pop(_lua, 2);
		_ptr->set_size(_s);

		return 0;
	};

	int lib_gfx::ltype_WidgetObject::get_size(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		auto _s = _ptr->get_size();

		lua_newtable(_lua);
		lua_pushinteger(_lua, _s.width);
		lua_setfield(_lua, -2, "w");
		lua_pushinteger(_lua, _s.height);
		lua_setfield(_lua, -2, "h");

		return 1;
	};

	lib_gfx::ltype_WidgetObject::pointer lib_gfx::ltype_WidgetObject::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_gfx::ltype_WidgetObject::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		lua::lua_inherit(_lua, ltype_GFXObject::tname(), -1);
		luaL_setfuncs(_lua, funcs, 0);
		lua_pop(_lua, 1);
		return 0;
	};
	

}

namespace sae::engine
{

	int lib_gfx::ltype_Sprite::new_f(lua_State* _lua)
	{
		auto _shader = lib_shader::to_shader(_lua, 1);
		auto _texture = lib_texture::to_texture(_lua, 2);
		auto _ptr = lua::lua_newinstance<value_type>(_lua, tname(), _shader, _texture);
		return 1;
	};

	lib_gfx::ltype_Sprite::pointer lib_gfx::ltype_Sprite::to_userdata(lua_State* _lua, int _idx)
	{
		return lua::lua_toinstance<value_type>(_lua, _idx, tname());
	};

	int lib_gfx::ltype_Sprite::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		lua::lua_inherit(_lua, ltype_WidgetObject::tname(), -1);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);
		return 1;
	};

}


namespace sae::engine
{

	int lib_gfx::lua_open(lua_State* _lua)
	{
		lua_newtable(_lua);

		assert(ltype_GFXObject::lua_open(_lua) == 0);
		assert(ltype_WidgetObject::lua_open(_lua) == 0);
		
		assert(ltype_Sprite::lua_open(_lua) == 1);
		lua_setfield(_lua, -2, "sprite");

		assert(luaopen_engine_gfx_rectangle(_lua) == 1);
		lua_setfield(_lua, -2, "rectangle");

		return 1;
	};

}