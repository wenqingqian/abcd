#pragma once
#include <pthread.h>
#include <unistd.h>
#include <functional>

#include <string>
#include "lock.hpp"
#include "common.hpp"
namespace abcd{
	
class thread : public noncopyable {
public:
	using threadFunc = std::function<void()>;

	thread(const threadFunc&, 
		const std::string& name = "default");
	~thread();

	void start();
	int join();
	bool started() const { return started_; }
	pid_t tid() const { return tid_; }
	const std::string& name() const { return name_; }

private:
	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	threadFunc func_;
	std::string name_;
	latch latch_;
};

}//namespace