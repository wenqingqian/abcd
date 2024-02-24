#pragma once
#include "noncopyable.hpp"
#include "logStream.hpp"
#include "lock.hpp"
#include "TomlConfig.hpp"
#include "logFile.hpp"
#include <mutex>
#include <vector>
#include <memory>

#include <iostream>
namespace abcd{

class asyncLogger : public noncopyable{
public:
	asyncLogger();

	void append(logStream &);

	void asyncLogFunc();

	void start(){
		thread_ = std::thread(&asyncLogger::asyncLogFunc,this);
		latch_.wait();
	}
	~asyncLogger(){
		cond_.notify_all();
		sleep(1);
		running_ = false;
		thread_.join();
	}

private:
	bool running_;
	int flush_val_;
	int fullsize_;
	int freesize_;
	int maxfullsize_;

	latch latch_;
	std::mutex mu_;
	condition cond_;
	std::thread thread_;
	using buffer = logBuffer<buffer_size_large>;
	using bufferptr = std::shared_ptr<buffer>;
	using buffervec = std::vector<bufferptr>;
	bufferptr current_bufferptr_;
	std::vector<bufferptr> fullqueue_;
	std::vector<bufferptr> freequeue_;
};
} // namespace abcd
