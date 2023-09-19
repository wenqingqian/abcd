#ifndef TSUKI_UTIL_LOGGER_HPP
#define TSUKI_UTIL_LOGGER_HPP

#include "noncopyable.hpp"
#include "LogStream.hpp"
#include "CurrentThread.hpp"
#include "Time.hpp"

#include <chrono>
#include <ctime>
#include <cstring>
#include <string>
#include <memory>
#include <source_location>
#include <filesystem>

namespace tsuki::util{

namespace fs = std::filesystem;

enum LogLevel{
	DEFAULT,
	DEBUG,
	INFO,
	WARN,
	ERROR,
	FATAL
};

class Logger : public noncopyable{
public:
	Logger(LogLevel, std::source_location = std::source_location::current());

	static LogLevel& loglevel() { return logignore_; }
	// static void setLoglevel(LogLevel level) { logignore_ = level; } 

	LogStream & stream() { return stream_; } 

	std::string getvalue(LogLevel);
	~Logger();


private:

	//当前日志级别
	LogLevel level_;
	std::string levelstr_;
	//缓冲区
	LogStream stream_;
	//源代码信息
	std::source_location sl_;

	//当前工作目录绝对路径
	static fs::path workpath_;
	//日志忽略级别
	static LogLevel logignore_;	
};

// class LogLevel{
// using string = std::string;
// public:
// 	enum LogLevel{
// 		DEFAULT,
// 		DEBUG,
// 		INFO,
// 		WARN,
// 		ERROR,
// 		FATAL
// 	};
// 	LogLevel(){}
// 	LogLevel(LogLevel);
// 	LogLevel(const LogLevel&);
// 	LogLevel(const char*);
// 	LogLevel(const string&);

// 	LogLevel level_;
// 	string levelstr_;

// 	string& value(){
// 		return levelstr_;
// 	} 
// };
}//namespace tsuki::util

#define LOG if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::INFO )\
	tsuki::util::Logger(tsuki::util::LogLevel::INFO).stream()
#define LOG_DEBUG if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::DEBUG )\
	tsuki::util::Logger(tsuki::util::LogLevel::DEBUG).stream()
#define LOG_INFO if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::INFO )\
	tsuki::util::Logger(tsuki::util::LogLevel::INFO).stream()
#define LOG_WARN if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::WARN )\
	tsuki::util::Logger(tsuki::util::LogLevel::WARN).stream()
#define LOG_ERROR if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::ERROR )\
	tsuki::util::Logger(tsuki::util::LogLevel::ERROR).stream()
#define LOG_FATAL if ( tsuki::util::Logger::loglevel() <= tsuki::util::LogLevel::FATAL )\
	tsuki::util::Logger(tsuki::util::LogLevel::FATAL).stream()


#endif