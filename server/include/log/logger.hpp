#pragma once

#include "noncopyable.hpp"
#include "logStream.hpp"
#include "currentThread.hpp"
#include "time.hpp"

#include <chrono>
#include <ctime>
#include <cstring>
#include <string>
#include <memory>
#include <source_location>
#include <filesystem>

namespace abcd{

namespace fs = std::filesystem;

enum logLevel {
	DEFAULT = 0,
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL
};

class logger : public noncopyable{
public:
	logger(logLevel, std::source_location = std::source_location::current());

	static logLevel& loglevel() { return logignore_; }
	// static void setLoglevel(LogLevel level) { logignore_ = level; } 

	logStream & stream() { return stream_; } 

	std::string getvalue(logLevel);
	~logger();


private:

	//当前日志级别
	logLevel level_;
	std::string levelstr_;
	//缓冲区
	logStream stream_;
	//源代码信息
	std::source_location sl_;

	//当前工作目录绝对路径
	static fs::path workpath_;
	//日志忽略级别
	static logLevel logignore_;	
};

#define LOG if ( logger::loglevel() <= logLevel::INFO )\
	logger(logLevel::INFO).stream()
#define LOG_DEBUG if ( logger::loglevel() <= logLevel::DEBUG )\
	logger(logLevel::DEBUG).stream()
#define LOG_INFO if ( logger::loglevel() <= logLevel::INFO )\
	logger(logLevel::INFO).stream()
#define LOG_WARN if ( logger::loglevel() <= logLevel::WARN )\
	logger(logLevel::WARN).stream()
#define LOG_ERROR if ( logger::loglevel() <= logLevel::ERROR )\
	logger(logLevel::ERROR).stream()
#define LOG_FATAL if ( logger::loglevel() <= logLevel::FATAL )\
	logger(logLevel::FATAL).stream()
#define LOG_TEST logger(logLevel::FATAL).stream()

} // namespace