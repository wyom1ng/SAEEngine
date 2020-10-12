#include "engine_file.h"

#include "SAELua.h"

namespace sae::engine
{

	bool File_Data::good() const noexcept
	{
		return this->fstr_.good();
	};
	bool File_Data::is_open() const noexcept
	{
		return this->fstr_.is_open();
	};

	void File_Data::open(const std::filesystem::path& _path)
	{
		this->fstr_.open(_path);
	};
	void File_Data::close()
	{
		this->fstr_.close();
	};

	void File_Data::seekg(size_t _pos)
	{
		this->fstr_.seekg((std::streampos)_pos);
	};
	void File_Data::seekg_offset(int _off)
	{
		this->fstr_.seekg(std::streamoff{ _off });
	};

	size_t File_Data::tellg()
	{
		return this->fstr_.tellg();
	};

	void File_Data::read(char* _out, size_t _maxCount)
	{
		this->fstr_.read(_out, _maxCount);
	};

	void File_Data::write(const char* _data, size_t _count)
	{
		this->fstr_.write(_data, _count);
	};

	void File_Data::flush()
	{
		this->fstr_.flush();
	};

	size_t File_Data::gcount()
	{
		return this->fstr_.gcount();
	};

	bool File_Data::eof() const
	{
		return this->fstr_.eof();
	};

	File_Data::File_Data(const std::filesystem::path& _path) : 
		path_{ _path }
	{};



	File_Data* lua_tofile(lua_State* _lua, int _idx, int _arg)
	{
		void* ud = lua::lua_downcast(_lua, _idx, "SAEEngine.file");
		luaL_argcheck(_lua, ud != NULL, _arg, "`file' expected");
		return (File_Data*)ud;
	};





	const luaL_Reg file_lib[] =
	{


		luaL_Reg{ NULL, NULL }
	};


	int luaopen_engine_file(lua_State* _lua)
	{
		lua::lua_newclass(_lua, "SAEEngine.file", file_lib);
		return 1;
	};

}