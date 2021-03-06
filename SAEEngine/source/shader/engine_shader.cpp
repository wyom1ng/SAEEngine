#include "engine_shader.h"

#include "SAELib_Either.h"
#include "SAELib_FileParse.h"

#include <fstream>
#include <string>
#include <optional>

#include <iostream>

namespace sae::engine
{

	sae::either<std::nullopt_t, glShaderError> check_shader(GLuint _id, std::nothrow_t) noexcept
	{
		GLint _result = GL_FALSE;
		int _ilogLen = 0;

		glGetShaderiv(_id, GL_COMPILE_STATUS, &_result);
		glGetShaderiv(_id, GL_INFO_LOG_LENGTH, &_ilogLen);

		if (_ilogLen > 0)
		{
			glShaderError _err{};
			_err.result = _result;
			_err.info_log.resize(_ilogLen + 1);
			glGetShaderInfoLog(_id, _ilogLen, NULL, _err.info_log.data());
			return { sae::other, _err };
		}
		else
		{
			return std::nullopt;
		};
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	sae::either<std::nullopt_t, glShaderError> check_shader(GLuint _id)
	{
		auto _out = check_shader(_id, std::nothrow);
		if (!_out)
		{
			throw std::runtime_error(_out.other().info_log);
		};
		return _out;
	};
#else
	sae::either<std::nullopt_t, glShaderError> check_shader(GLuint _id) noexcept
	{
		return check_shader(_id, std::nothrow);
	};
#endif

	sae::either<std::nullopt_t, glShaderError> check_program(GLuint _id, std::nothrow_t) noexcept
	{
		GLint _result = GL_FALSE;
		int _ilogLen = 0;

		glGetProgramiv(_id, GL_LINK_STATUS, &_result);
		glGetProgramiv(_id, GL_INFO_LOG_LENGTH, &_ilogLen);

		if (_ilogLen > 0)
		{
			glShaderError _err{};
			_err.result = _result;
			_err.info_log.resize(_ilogLen + 1);
			glGetProgramInfoLog(_id, _ilogLen, NULL, _err.info_log.data());
			return { sae::other, _err };
		}
		else
		{
			return std::nullopt;
		};
	};

#ifdef SAE_ENGINE_USE_EXCEPTIONS
	sae::either<std::nullopt_t, glShaderError> check_program(GLuint _id)
	{
		auto _out = check_program(_id, std::nothrow);
		if (!_out)
		{
			throw std::runtime_error(_out.other().info_log);
		};
		return _out;
	};
#else
	sae::either<std::nullopt_t, glShaderError> check_program(GLuint _id) noexcept
	{
		return check_program(_id, std::nothrow);
	};
#endif

	sae::either<std::nullopt_t, glShaderError> compile_shader(GLuint _id, const char* _src) noexcept(noexcept(check_shader(0)))
	{
		glShaderSource(_id, 1, &_src, NULL);
		glCompileShader(_id);

		return check_shader(_id);
	};
	sae::either<GLuint, glShaderError> build_shader_object(std::initializer_list<GLuint> _shaderIDs) noexcept(noexcept(check_program(0)))
	{
		// Attach shaders
		GLuint _programID = glCreateProgram();
		for (auto& p : _shaderIDs)
		{
			glAttachShader(_programID, p);
		};
		
		// Link program
		glLinkProgram(_programID);

		// Check object
		auto _res = check_program(_programID);
		if (!_res)
		{
			return { sae::other, std::move(_res.other()) };
		};

		// Detach shaders
		for (auto& p : _shaderIDs)
		{
			glDetachShader(_programID, p);
		};

		// Delete individual programs
		for (auto& p : _shaderIDs)
		{
			glDeleteShader(p);
		};

		return _programID;
	};



	const std::string& Shader_Data::name() const noexcept
	{
		return this->name_;
	};
	GLuint Shader_Data::id() const noexcept
	{
		return this->id_;
	};

	bool Shader_Data::good() const noexcept
	{
		return (this->id() != 0);
	};
	Shader_Data::operator bool() const noexcept
	{
		return this->good();
	};

	void Shader_Data::bind() const
	{
		glUseProgram(this->id());
	};
	void Shader_Data::unbind() const
	{
		glUseProgram(0);
	};

	void Shader_Data::destroy()
	{
		if (this->good())
		{
			glDeleteShader(this->id());
			this->id_ = 0;
		};
	};

	Shader_Data::Shader_Data(const std::string& _name, const std::filesystem::path& _vertexPath, const std::filesystem::path& _fragmentPath) :
		name_{ _name }
	{
		
		GLuint _vertID = glCreateShader(GL_VERTEX_SHADER);
		auto _res = compile_shader(_vertID, file_reader{ _vertexPath }().c_str());
		if (!_res)
		{
			return;
		};

		GLuint _fragID = glCreateShader(GL_FRAGMENT_SHADER);
		_res = compile_shader(_fragID, file_reader{ _fragmentPath }().c_str());
		if (!_res)
		{
			return;
		};

		auto _buildRes = build_shader_object({ _vertID, _fragID });
		if (!_res)
		{
			return;
		}
		else
		{
			this->id_ = *_buildRes;
		};
		
	};
	Shader_Data::~Shader_Data()
	{
#ifdef SAE_ENGINE_DESTRUCTOR_DEBUG
		std::cout << "~Shader_Data()\n";
#endif
		this->destroy();
	};

}

namespace sae::engine
{

	// engine.shader.new(name, vertexPath, fragmentPath) -> Shader_Data
	int lib_shader::ltype_Shader::new_f(lua_State* _lua)
	{
		auto _name = lua_tostring(_lua, -3);
		auto _vertexPath = lua_tostring(_lua, -2);
		auto _fragmentPath = lua_tostring(_lua, -1);
		auto _ptr = lua::lua_newinstance<value_type>(_lua, tname(), _name, _vertexPath, _fragmentPath);
		return 1;
	};


	// shader:good() -> bool
	int lib_shader::ltype_Shader::good(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		lua_pushboolean(_lua, _ptr->good());
		return 1;
	};

	// shader:name() -> string
	int lib_shader::ltype_Shader::name(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		lua_pushstring(_lua, _ptr->name().c_str());
		return 1;
	};

	// shader:destroy()
	int lib_shader::ltype_Shader::destroy(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->destroy();
		return 0;
	};

	// shader:__gc()
	int lib_shader::ltype_Shader::destructor(lua_State* _lua)
	{
		auto _ptr = to_userdata(_lua, 1);
		_ptr->~Shader_Data();
		return 0;
	};


	lib_shader::ltype_Shader::pointer lib_shader::ltype_Shader::to_userdata(lua_State* _lua, int _arg)
	{
		return lua::lua_toinstance<value_type>(_lua, _arg, tname());
	};


	int lib_shader::ltype_Shader::lua_open(lua_State* _lua)
	{
		lua::lua_newclass(_lua, tname());
		luaL_setfuncs(_lua, funcs_m, 0);
		lua_pop(_lua, 1);

		lua_newtable(_lua);
		luaL_setfuncs(_lua, funcs_f, 0);

		return 1;
	};


}

namespace sae::engine
{

	int lib_shader::lua_open(lua_State* _lua)
	{
		return ltype_Shader::lua_open(_lua);
	};

}