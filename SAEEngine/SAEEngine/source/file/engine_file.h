#pragma once

#include "SAEEngine.h"


#include <filesystem>
#include <fstream>

namespace sae::engine
{
	struct File_Data
	{
	public:

		bool good() const noexcept;
		bool is_open() const noexcept;

		void open(const std::filesystem::path& _path);
		void close();

		void seekg(size_t _pos);
		void seekg_offset(int _off);

		size_t tellg();

		void read(char* _out, size_t _maxCount);
		void write(const char* _data, size_t _count);

		size_t gcount();

		void flush();

		bool eof() const;

		File_Data() = default;
		File_Data(const std::filesystem::path& _path);

	private:
		std::filesystem::path path_;
		std::fstream fstr_;
	};


	File_Data* lua_tofile(lua_State* _lua, int _idx, int _arg);

	// engine.file.new([path])
	int file_new(lua_State* _lua);

	// file:open(path)
	int file_open(lua_State* _lua);

	// file:is_open() -> bool
	int file_is_open(lua_State* _lua);




	// file:close()
	int file_close(lua_State* _lua);




	// file:__gc()
	int file_destructor(lua_State* _lua);


	int luaopen_engine_file(lua_State* _lua);

}