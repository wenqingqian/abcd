#include "logger.hpp"
#include "TomlConfig.hpp"
#include <pthread.h>
#include "asyncLogger.hpp"
#include "reflect.hpp"
#include "config.hpp"
#include <iostream>
namespace abcd{

static pthread_once_t once_control_ = PTHREAD_ONCE_INIT;
static asyncLogger *asyncLogger_;
// __thread pthread_once_t tidexist = PTHREAD_ONCE_INIT;
// __thread pid_t thread_id = 0;
// __thread char* thread_name = "default";

void once_init(){
    asyncLogger_ = new asyncLogger();
    asyncLogger_->start(); 
}

fs::path logger::workpath_ = fs::current_path();

logLevel logger::logignore_ = [](){
	auto str = conf.log.level;
	output("logignore "<<str);
	auto level = name2enum<logLevel>(str);
	// auto level = DEFAULT;
	// if(str == "FATAL") level = FATAL;
	// else if(str == "ERROR") level = ERROR;
	// else if(str == "WARN" ) level = WARN;
	// else if(str == "INFO" ) level = INFO;
	// else if(str == "DEBUG") level = DEBUG;
	return level;
}();

logger::logger(logLevel level, std::source_location sl)
	:	stream_(static_cast<int>(level)),
		level_(level),
	 	sl_(sl),
	 	levelstr_(getvalue(level))
	{
	mtime t(mtime::year,mtime::microsecond);

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

std::string logger::getvalue(logLevel level){
	auto res = enum2name(level);
	auto app = 5-res.size();
	if(app>0){
		res+=' ';
		app--;
	}
	return res;
}

logger::~logger(){
	stream_.append(" ",1);
	stream_ << fs::relative(sl_.file_name(),workpath_) << ":" << sl_.line() << '\n';
	pthread_once(&once_control_, once_init);
#if defined(LOG_PUSH_TO_TERMINAL)
	std::cout<< stream_.release() <<std::endl;
#else
	asyncLogger_->append(stream_); 
#endif
}

void stopAsyncLogger(){
	delete asyncLogger_;
}

}
