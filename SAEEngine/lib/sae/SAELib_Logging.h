#pragma once

#include "SAELib_Concepts.h"

#include <ostream>
#include <fstream>
#include <string>
#include <atomic>
#include <concepts>
#include <filesystem>
#include <exception>

namespace sae
{

	using log_message_t = std::string;

	struct endentry_t {};
	const static inline endentry_t endentry{};

	struct log_entry
	{
		std::string level = "";
		std::string source = "";
		std::string message = "";
	};

	struct basic_log_formatter
	{
		log_message_t format(const log_entry& _entry) const
		{
			return "(" + _entry.level + ")[" + _entry.source + "] " + _entry.message;
		};
	};

	struct LoggerException : public std::exception
	{
		using std::exception::exception;
		using std::exception::operator=;
	};
	
	struct basic_logger
	{
	public:
		std::ostream& log(std::ostream& _ostr, const log_message_t& _msg)
		{
			_ostr << _msg;
			return _ostr;
		};
	};

	struct basic_threadsafe_logger : public basic_logger
	{
	public:
		std::ostream& log(std::ostream& _ostr, const log_message_t& _msg)
		{
			while (flag_.test_and_set()) {};
			basic_logger::log(_ostr, _msg);
			flag_.clear();
			return _ostr;
		};
		using basic_logger::log;
	private:
		static inline std::atomic_flag flag_ = ATOMIC_FLAG_INIT;
	};

	template <typename T>
	concept cx_log_entry_formatter = requires (T a, const log_entry& _entry)
	{
		{ a.format(_entry) } -> cx_same_as<log_message_t>;
	};

	struct file_logger
	{
	public:
		const std::filesystem::path& path() const
		{
			return this->path_;
		};

		file_logger& log(const log_message_t& _message)
		{
			this->ofstr_ << _message;
			return *this;
		};
		file_logger& log(endentry_t)
		{
			this->ofstr_ << std::endl;
			return *this;
		};

		bool is_open() const
		{
			return this->ofstr_.is_open();
		};
		explicit operator bool() const { return this->is_open(); };

		void close()
		{
			if (this->ofstr_.is_open())
				this->ofstr_.close();
		};
		void open(const std::filesystem::path& _path)
		{
			this->close();
			this->path_ = _path;
			this->ofstr_.open(this->path(), std::ios::ate | std::ios::app);
			this->log("LOG BEGIN").log(endentry);
		};
		void clear()
		{
			bool _o = this->is_open();
			this->close();
			std::filesystem::remove(this->path());
			if (_o)
				this->ofstr_.open(this->path(), std::ios::ate | std::ios::app);
		};

		file_logger& operator<<(const log_entry& _entry)
		{
			return this->log(basic_log_formatter{}.format(_entry));
		};
		file_logger& operator<<(const log_message_t& _msg)
		{
			return this->log(_msg);
		};
		file_logger& operator<<(const endentry_t& _end)
		{
			return this->log(_end);
		};

		file_logger() = default;
		file_logger(const std::filesystem::path& _path) : 
			path_{ (_path.is_relative())? _path : std::filesystem::relative(_path) }
		{
			this->open(this->path());
		};

		~file_logger()
		{
			this->close();
		};

	private:
		std::filesystem::path path_{};
		std::ofstream ofstr_{};
	};

	template <typename T>
	concept cx_logger = requires (T a, const log_message_t& _msg)
	{
		{ a.log(_msg) } -> cx_same_as<typename T&>;
		{ a.log(endentry_t{}) } -> cx_same_as<typename T&>;
	};
	
}