#pragma once
#include "eventloop.h"
#include "common.hpp"
#include "lock.hpp"

namespace abcd{

class eventloopThread : public noncopyable {
public:
	eventloopThread();
	~eventloopThread();

	eventloop* startLoop();

private:
	void threadFunc();
	
	eventloop* loop_;
	bool exiting_;
	std::thread thread_; // 可能需要自定义一个thread类型处理(用pthread)
	std::mutex mutex_;
	condition cond_;
};

}