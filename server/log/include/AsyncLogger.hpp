#ifndef TSUKI_UTIL_ASYNCLOGGER_HPP
#define TSUKI_UTIL_ASYNCLOGGER_HPP 1

#include "noncopyable.hpp"
#include "LogStream.hpp"
#include "SyncUtil.hpp"
#include "TomlConfig.hpp"
#include "LogFile.hpp"
#include <mutex>
#include <vector>
#include <memory>

#include <iostream>
namespace tsuki::util{

class AsyncLogger : public noncopyable{
public:
	AsyncLogger();

	void append(LogStream &);

	void asyncLogFunc();

	void start(){
		thread_ = std::thread(&AsyncLogger::asyncLogFunc,this);
		std::cout<<"pre start"<<std::endl;
		latch_.wait();
		std::cout<<"start ok"<<std::endl;
	}
	~AsyncLogger(){
		running_ = false;
		thread_.join();
	}

private:
	bool running_;
	int flush_val_;
	int fullsize_;
	int freesize_;
	int maxfullsize_;
	config::config_& configAsync;

	AtomicLatch latch_;
	std::mutex mu_;
	Condition cond_;
	std::thread thread_;
	using buffer = LogBuffer<buffer_size_large>;
	using bufferptr = std::shared_ptr<buffer>;
	using buffervec = std::vector<bufferptr>;
	bufferptr current_bufferptr_;
	std::vector<bufferptr> fullqueue_;
	std::vector<bufferptr> freequeue_;
};
} // namespace tsuki::util

#endif