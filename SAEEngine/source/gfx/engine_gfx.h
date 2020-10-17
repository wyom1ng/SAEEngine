#pragma once

#include "SAEEngine.h"
#include "SAEEngine_BaseTypes.h"

#include "shader/engine_shader.h"
#include "texture/engine_texture.h"

#include <glm/gtc/matrix_transform.hpp>
#include <array>
#include <memory>

namespace sae::engine
{

	struct vertex_array_object
	{
		bool good() const noexcept;
		explicit operator bool() const noexcept;

		void bind();
		void unbind();

		void destroy();

		vertex_array_object();
		~vertex_array_object();
		
		GLuint id = 0;
	};

	struct vertex_buffer_object
	{
		bool good() const noexcept;
		explicit operator bool() const noexcept;

		void bind(GLenum _target);
		void unbind(GLenum _target);

		template <typename T>
		void buffer_data(GLenum _target, size_t _count, T* _data, GLenum _usage)
		{
			this->bind(_target);
			glBufferData(_target, sizeof(T) * _count, _data, _usage);
		};

		template <typename T>
		void buffer_subdata(GLenum _target, size_t _offset, size_t _count, T* _data)
		{
			this->bind(_target);
			glBufferSubData(_target, sizeof(T) * _offset, sizeof(T) * _count, _data);
		};

		void destroy();

		vertex_buffer_object();

		vertex_buffer_object(const vertex_buffer_object& other) = delete;
		vertex_buffer_object& operator=(const vertex_buffer_object& other) = delete;
		
		vertex_buffer_object(vertex_buffer_object&& other) noexcept :
			id{ std::exchange(other.id, 0) }
		{};
		vertex_buffer_object& operator=(vertex_buffer_object&& other) noexcept
		{
			this->destroy();
			this->id = std::exchange(other.id, 0);
			return *this;
		};


		~vertex_buffer_object();

		GLuint id = 0;
	};

	template <typename T, typename _Alloc = std::allocator<T>>
	struct vertex_data_array
	{
	public:

		using value_type = T;
		using allocator_type = _Alloc;

		using pointer = value_type*;
		using reference = value_type&;
		using const_pointer = const value_type*;
		using const_reference = const value_type&;


		bool good() const noexcept
		{
			return this->vbo_.good();
		};
		explicit operator bool() const noexcept
		{
			return this->good();
		};

		void bind()
		{
			this->vbo_.bind(this->target_);
		};
		void unbind()
		{
			this->vbo_.unbind(this->target_);
		};

		void destroy()
		{
			if (this->good())
			{
				this->vbo_.destroy();
			};
		};



		size_t size() const noexcept
		{
			return this->size_;
		};
		size_t capacity() const noexcept
		{
			return this->capacity_;
		};

		pointer data() const noexcept { return this->data_; };

		reference back() { return *(this->data() + this->size() - 1); };
		reference front() { return *(this->data()); };
		const_reference back() const { return *(this->data() + this->size() - 1); };
		const_reference front() const { return *(this->data()); };

		void reserve(size_t _count)
		{
			auto _ptr = this->alloc_.allocate(_count);

			auto _in = this->data();
			auto _inEnd = _in + std::min(this->size(), _count);
			for (auto _out = _ptr; _in < _inEnd; ++_in, ++_out)
			{
				this->alloc_.construct(_out, std::move(*_in));
			};

			this->alloc_.deallocate(this->data(), this->capacity());
			this->data_ = _ptr;

			this->capacity_ = _count;
			if (this->size() > _count)
				this->size_ = _count;

			//this->vbo_.buffer_data(this->target_, this->size(), this->data(), this->usage_);

		};
		void resize(size_t _count)
		{
			auto _out = (&this->back()) + 1;
			auto _outEnd = this->data() + std::min(this->size(), _count);
			if (_count > this->size())
			{
				if (_count > this->capacity())
					this->reserve(_count);

				for (_out; _out != _outEnd; ++_out)
				{
					this->alloc_.construct(_out, value_type{});
				};
			}
			else
			{
				for (_out; _out != _outEnd; --_out)
				{
					this->alloc_.destroy(_out);
				};
			};
			this->size_ = _count;
		};

		void push_back(const_reference _v)
		{
			if (this->size() == this->capacity())
			{
				this->reserve(this->capacity() << 1);
			};
			this->alloc_.construct(this->data() + this->size() + 1, _v);
		};
		void push_back(reference& _v)
		{
			if (this->size() == this->capacity())
			{
				this->reserve(this->capacity() << 1);
			};
			this->alloc_.construct(this->data() + this->size() + 1, std::move(_v));
		};

		reference at(size_t _p) { return *(this->data() + _p); };
		const_reference at(size_t _p) const { return *(this->data() + _p); };

		reference operator[](size_t _p) { return this->at(_p); };
		const_reference operator[](size_t _p) const { return this->at(_p); };

		void clear() noexcept(noexcept(this->alloc_.destroy(nullptr)))
		{
			auto _end = this->data() + this->size();
			for (auto p = this->data(); p != _end; ++p)
			{
				this->alloc_.destroy(p);
			};
			this->size_ = 0;
		};

		void realloc_buffer()
		{
			this->vbo_.buffer_data(this->target_, this->size(), this->data(), this->usage_);
		};
		void update(size_t _offset, size_t _count)
		{
			assert(_offset + _count <= this->size());
			this->vbo_.buffer_subdata(this->target_, _offset, _count, this->data());
		};



		vertex_data_array(GLenum _target, GLenum _usage) :
			target_{ _target }, usage_{ _usage }
		{};
		vertex_data_array(GLenum _target, GLenum _usage, std::initializer_list<T> _itList) :
			target_{ _target }, usage_{ _usage }, data_{ this->alloc_.allocate(_itList.size()) },
			size_{ _itList.size() }, capacity_{ _itList.size() }
		{
			auto _out = this->data();
			for (auto& p : _itList)
			{
				std::construct_at(_out, std::move(p));
				++_out;
			};
		};
		vertex_data_array(const vertex_data_array& other) :
			target_{ other.target_ }, usage_{ other.usage_ }
		{
			this->reserve(other.capacity());
			this->size_ = other.size();
			auto _in = other.data();
			auto _inEnd = _in + other.size();
			for (auto _out = this->data(); _in != _inEnd; ++_in, ++_out)
			{
				this->alloc_.construct(_out, *_in);
			};
		};
		vertex_data_array& operator=(const vertex_data_array& other)
		{
			this->resize(other.size());
			auto _in = other.data();
			auto _inEnd = _in + other.size();
			for (auto _out = this->data(); _in != _inEnd; ++_in, ++_out)
			{
				(*_out) = (*_in);
			};
			return *this;
		};
		vertex_data_array(vertex_data_array&& other) noexcept :
			target_{ other.target_ }, usage_{ other.usage_ },
			vbo_{ std::move(other.vbo_) }, data_{ std::exchange(other.data_, nullptr) },
			size_{ std::exchange(other.size_, 0) }, capacity_{ std::exchange(other.capacity_, 0) }
		{};
		vertex_data_array& operator=(vertex_data_array&& other) noexcept
		{
			this->destroy();
			this->size_ = std::exchange(other.size_, 0);
			this->capacity_ = std::exchange(other.capacity_, 0);
			this->data_ = std::exchange(other.data_, nullptr);
			this->vbo_ = std::move(other.vbo_);
			return *this;
		};

		~vertex_data_array()
		{
			this->destroy();
			this->alloc_.deallocate(this->data(), this->capacity());
		};

	private:
		allocator_type alloc_{};
		vertex_buffer_object vbo_{};
		const GLenum target_;
		const GLenum usage_;
		pointer data_ = nullptr;
		size_t size_ = 0;
		size_t capacity_ = 0;
	};


	struct vertex_attribute
	{
		void enable() const;
		void disable() const;

		void set() const;

		vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize, uint16_t _stride, uint16_t _offset);
		vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize, uint16_t _stride);
		vertex_attribute(GLuint _index, uint8_t _size, GLenum _type, bool _normalize);
		vertex_attribute(GLuint _index, uint8_t _size, GLenum _type);

		const bool normalize = false;
		const uint8_t size = 0;
		const uint16_t stride = 0;
		const uint16_t offset = 0;
		const GLuint index = 0;
		const GLenum type = GL_FLOAT;
	};
	

	
	struct Widget_Rectangle : public WidgetObject
	{
	public:
		bool good() const;
		void update();
		void draw(const glm::mat4& _projectionMat);
		void destroy();

		Widget_Rectangle(Shader_Data* _shader);
		~Widget_Rectangle();

	protected:
		Shader_Data* shader_ = nullptr;

		GLuint projection_uniform_ = 0;

		vertex_array_object vao_{};
		vertex_attribute pos_attr_{ 0, 3, GL_FLOAT };
		vertex_data_array<float_t> pos_
		{
			GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				0.0f, 0.0f, 0.0f,
				0.0f, 100.0f, 0.0f,
				100.0f, 0.0f, 0.0f,
				100.0f, 100.0f, 0.0f
			}
		};

		vertex_attribute col_attr_{ 1, 4, GL_UNSIGNED_BYTE, true };
		vertex_data_array<uint8_t> cols_
		{
			GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				255, 255, 255, 255,
				255, 255, 255, 255,
				255, 255, 255, 255,
				255, 255, 255, 255
			}
		};

		vertex_data_array<uint8_t> indices_
		{
			GL_ELEMENT_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				0, 1, 2,
				1, 3, 2
			}
		};

	};

	struct Widget_Sprite : public WidgetObject
	{
	public:
		bool good() const;
		void update();
		void draw(const glm::mat4& _projectionMat);
		void destroy();

		Widget_Sprite(Shader_Data* _shader, Texture* _tex);
		~Widget_Sprite();

	private:
		Shader_Data* shader_ = nullptr;

		GLuint texture_id_ = 0;
		Texture* image_ = nullptr;

		GLuint projection_uniform_ = 0;

		vertex_array_object vao_{};
		vertex_attribute pos_attr_{ 0, 3, GL_FLOAT };
		vertex_data_array<float_t> pos_
		{
			GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				0.0f, 0.0f, 0.0f,
				0.0f, 100.0f, 0.0f,
				100.0f, 0.0f, 0.0f,
				100.0f, 100.0f, 0.0f
			}
		};

		vertex_attribute col_attr_{ 1, 4, GL_UNSIGNED_BYTE, true };
		vertex_data_array<uint8_t> cols_
		{
			GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				255, 255, 255, 255,
				255, 255, 255, 255,
				255, 255, 255, 255,
				255, 255, 255, 255
			}
		};

		vertex_attribute uv_attr_{ 2, 2, GL_FLOAT };
		vertex_data_array<float_t> uvs_
		{
			GL_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				0.0f, 0.0f,
				0.0f, 1.0f,
				1.0f, 0.0f,
				1.0f, 1.0f
			}
		};

		vertex_data_array<uint8_t> indices_
		{
			GL_ELEMENT_ARRAY_BUFFER,
			GL_STATIC_DRAW,
			{
				0, 1, 2,
				1, 3, 2
			}
		}; 

	};









	Widget_Rectangle* lua_torectangle(lua_State* _lua, int _idx, int _arg);

	// gfx.rectangle.new(Shader_Data) -> rectangle
	int gfx_rectangle_new(lua_State* _lua);

	static inline const luaL_Reg gfx_rectangle_lib[] =
	{
		luaL_Reg{ "new", &gfx_rectangle_new },
		luaL_Reg{ NULL, NULL }
	};
	int luaopen_engine_gfx_rectangle(lua_State* _lua);




	

	struct lib_gfx
	{
	public:

		struct ltype_GFXObject
		{
		private:

			static int good(lua_State* _lua);
			
			static int update(lua_State* _lua);

			static int destructor(lua_State* _lua);

			constexpr static inline luaL_Reg funcs[] =
			{
				luaL_Reg{ "update", &update },
				luaL_Reg{ "good", &good },
				luaL_Reg{ "__gc", &destructor },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.GFXObject";

		public:
			using value_type = GFXObject;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			constexpr static inline const char* tname() noexcept
			{
				return TYPENAME;
			};

			static int lua_open(lua_State* _lua);

		};

		struct ltype_WidgetObject
		{
		private:

			static int set_color(lua_State* _lua);
			static int get_color(lua_State* _lua);

			static int set_position(lua_State* _lua);
			static int get_position(lua_State* _lua);

			static int set_size(lua_State* _lua);
			static int get_size(lua_State* _lua);

			constexpr static inline luaL_Reg funcs[] =
			{
				luaL_Reg{ "set_color", &set_color },
				luaL_Reg{ "get_color", &get_color },
				luaL_Reg{ "set_position", &set_position },
				luaL_Reg{ "get_position", &get_position },
				luaL_Reg{ "set_size", &set_size },
				luaL_Reg{ "get_size", &get_size },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.WidgetObject";

		public:
			using value_type = WidgetObject;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			constexpr static inline const char* tname() noexcept 
			{
				return TYPENAME;
			};

			static int lua_open(lua_State* _lua);

		};


		struct ltype_Sprite
		{
		private:

			static int new_f(lua_State* _lua);

			constexpr static inline luaL_Reg funcs_f[] = 
			{
				luaL_Reg{ "new", &new_f },
				luaL_Reg{ NULL, NULL }
			};

			constexpr static inline auto TYPENAME = "SAEEngine.sprite";

		public:

			using value_type = Widget_Sprite;
			using pointer = value_type*;

			static pointer to_userdata(lua_State* _lua, int _idx);

			constexpr static inline const char* tname() noexcept { return TYPENAME; };

			static int lua_open(lua_State* _lua);

		};



		struct WorldObject
		{

		};


	private:








	public:
		
		static inline auto to_gfxobject(lua_State* _lua, int _arg)
		{
			return ltype_GFXObject::to_userdata(_lua, _arg);
		};
		static inline auto to_widgetobject(lua_State* _lua, int _arg)
		{
			return ltype_WidgetObject::to_userdata(_lua, _arg);
		};



		static int lua_open(lua_State* _lua);

	};



}