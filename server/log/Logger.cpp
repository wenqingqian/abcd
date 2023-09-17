#include "Logger.hpp"
#include "TomlConfig.hpp"
#include <pthread.h>
#include "AsyncLogger.hpp"
#include "CommonUtil.hpp"
#include <iostream>
namespace tsuki::util{

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static AsyncLogger *AsyncLogger_;
// __thread pthread_once_t tidexist = PTHREAD_ONCE_INIT;
// __thread pid_t thread_id = 0;
// __thread char* thread_name = "default";

void once_init(){
    AsyncLogger_ = new AsyncLogger();
    AsyncLogger_->start(); 
}

fs::path Logger::workpath_ = fs::current_path();

LogLevel Logger::logignore_ = [](){
	auto res = CONFIG["log"]["level"].value<config::String>();
	assert(res);
	auto str = *res;
	auto level = DEFAULT;
	if(str == "FATAL") level = FATAL;
	else if(str == "ERROR") level = ERROR;
	else if(str == "WARN" ) level = WARN;
	else if(str == "INFO" ) level = INFO;
	else if(str == "DEBUG") level = DEBUG;
	return level;
}();

Logger::Logger(LogLevel level, std::source_location sl)
	:	stream_(static_cast<int>(level)),
		level_(level),
	 	sl_(sl),
	 	levelstr_(getvalue(level))
	{
	time t(time::year,time::microsecond);

	//time
	stream_.append(t.getTime(), t.length());
	stream_.append(" ",1);
	//time level
	stream_.append(levelstr_);
	stream_.append(" ",1);
	//time level thread
	stream_.append(currentThread::t_id_string_cache_);
	stream_.append(" ",1);
	//time level thread thread_name
	stream_.append(currentThread::t_name_cache_);
	stream_.append(" ",1);
}

std::string Logger::getvalue(LogLevel level){
	auto s = reflect::enumName(level);
	return s.substr(s.find_last_of(':')+1);
}

Logger::~Logger(){
	// std::cout<<"~logger"<<std::endl;
	stream_.append(" ",1);
	stream_ << fs::relative(sl_.file_name(),workpath_) << ":" << sl_.line() << '\n';
	pthread_once(&once_control_, once_init);
	AsyncLogger_->append(stream_); 
}


/*
switch (level){
		case DEBUG: return "DEBUG";break;
		case INFO : return "INFO ";break;
		case WARN : return "WARN ";break;
		case ERROR: return "ERROR";break;
		case FATAL: return "FATAL";break;
	}
	return "NULL";
*/





// LogLevel::LogLevel(LogLevel l){
// 	level_ = l;
// 	switch (l){
// 		case DEBUG: levelstr_ = "DEBUG";break;
// 		case INFO : levelstr_ = "INFO ";break;
// 		case WARN : levelstr_ = "WARN ";break;
// 		case ERROR: levelstr_ = "ERROR";break;
// 		case FATAL: levelstr_ = "FATAL";break;
// 	}
// }
// LogLevel::LogLevel(const LogLevel& l){
// 	level_ = l.level_;
// 	levelstr_ = l.levelstr_;
// }
// LogLevel::LogLevel(const char* l){
// 	levelstr_ = l;
// 	if(strcmp(l,"FATAL")) level_ = FATAL;
// 	else if(strcmp(l,"ERROR")) level_ = ERROR;
// 	else if(strcmp(l,"WARN" )) level_ = WARN;
// 	else if(strcmp(l,"INFO" )) level_ = INFO;
// 	else if(strcmp(l,"DEBUG")) level_ = DEBUG;
// 	else level_ = DEFAULT;
// }

// LogLevel::LogLevel(const string& str){
// 	levelstr_ = str;
// 	if(str == "FATAL") level_ = FATAL;
// 	else if(str == "ERROR") level_ = ERROR;
// 	else if(str == "WARN" ) level_ = WARN;
// 	else if(str == "INFO" ) level_ = INFO;
// 	else if(str == "DEBUG") level_ = DEBUG;
// 	else level_ = DEFAULT;
// }

}//namespace tsuki::util

