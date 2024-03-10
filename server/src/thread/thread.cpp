#include <iostream>
#include <errno.h>
#include <linux/unistd.h>
#include <stdint.h>
#include <stdio.h>
#include <sys/prctl.h>
#include <sys/types.h>
#include <unistd.h>
#include <memory>
#include "currentThread.hpp"
#include "thread.hpp"

namespace abcd{
	
struct threaddata {
	using threadFunc = std::function<void()>;

	threadFunc func_;
	string name_;
	pid_t* tid_;
	latch* latch_;

	threaddata(const threadFunc& func, const string& name, pid_t* tid, latch* latch)
		: func_(func), name_(name), tid_(tid), latch_(latch) 
		{}

	void runInthread() {
		currentThread::this_thread_init_once();
		*tid_ = currentThread::t_id_cache_;
		tid_ = nullptr;
		latch_->count_down();
		latch_ = nullptr;

		currentThread::t_name_cache_ = name_.empty() ? "thread" : name_.c_str();
		prctl(PR_SET_NAME, currentThread::t_name_cache_);

		func_();
		currentThread::t_name_cache_ = "finish";
	}
};

void* startthread(void* obj) {
	threaddata* data = static_cast<threaddata*>(obj);
	data->runInthread();
	delete data;
	return nullptr;
}

thread::thread(const threadFunc& func, const string& n)
	:	started_(false),
		joined_(false),
		pthreadId_(0),
		tid_(0),
		func_(func),
		name_(n),
		latch_(1) 
	{}

thread::~thread() {
	if (started_ && !joined_) pthread_detach(pthreadId_);
}


void thread::start() {
	assert(!started_);
	started_ = true;
	threaddata* data = new threaddata(func_, name_, &tid_, &latch_);
	if (pthread_create(&pthreadId_, nullptr, &startthread, data)!=0) {
		started_ = false;
		delete data;
	} else {
		latch_.wait();
		assert(tid_ > 0);
	}
}

int thread::join() {
	assert(started_);
	assert(!joined_);
	joined_ = true;
	return pthread_join(pthreadId_, nullptr);
}

}//namespace