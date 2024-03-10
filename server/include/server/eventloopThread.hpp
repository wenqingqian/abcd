// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include "eventloop.hpp"
#include "lock.hpp"
#include "thread.hpp"
namespace abcd{

class eventloopThread : noncopyable {
public:
	eventloopThread();
	~eventloopThread();
	eventloop* startLoop();

private:
	void threadFunc();
	eventloop* loop_;
	bool exiting_;
	thread thread_;
	std::mutex mutex_;
	condition cond_;
};

}// namespace