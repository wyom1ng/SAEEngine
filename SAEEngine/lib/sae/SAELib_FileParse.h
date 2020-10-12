#pragma once

#include <filesystem>
#include <fstream>
#include <string>
#include <sstream>

namespace sae
{



	struct file_reader
	{
	public:

		std::string read_all()
		{
			std::string _out{};
			if (this->fstr_.is_open())
			{
				char _buff[64]{ 0 };
				this->fstr_.seekg(0);
				while (!this->fstr_.eof())
				{
					this->fstr_.read(_buff, sizeof(_buff) / sizeof(char));
					_out.append(_buff, this->fstr_.gcount());
				};
			};
			return _out;
		};
		std::string operator()()
		{
			return this->read_all();
		};

		file_reader(const std::filesystem::path& _path) :
			path_{ (_path.is_relative())? _path : std::filesystem::relative(_path) }, fstr_{ _path }
		{};

	private:
		std::filesystem::path path_{};
		std::fstream fstr_{};
	};





}