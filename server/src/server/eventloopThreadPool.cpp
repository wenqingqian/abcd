#include "eventloopThreadPool.h"

namespace abcd{

// 这个类的目的是创建一个包含多个线程的线程池，每个线程都有一个独立的 eventloop 对象。
// 在构造函数中，指定了基础 eventloop（即主线程的 eventloop）和线程数。
// 在 start 函数中，启动了指定数量的线程，每个线程都有一个独立的 eventloop 对象。
// 在 getNextLoop 函数中，根据轮询策略获取下一个要使用的 eventloop。

// 构造函数
eventloopThreadPool::eventloopThreadPool(eventloop *baseLoop, int numThreads)
	:	baseLoop_(baseLoop), 
		started_(false), 
		numThreads_(numThreads), 
		next_(0) 
	{
	if (numThreads_ <= 0) {
		LOG << "numThreads_ <= 0";
		abort();
	}
}

// 启动线程池
void eventloopThreadPool::start() {
	baseLoop_->assertInLoopThread();  // 确保在 baseLoop 所在的线程中调用
	started_ = true;  // 标记线程池已启动
	for (int i = 0; i < numThreads_; ++i) {
		std::shared_ptr<eventloopThread> t(new eventloopThread());  // 创建 eventloopThread 对象
		threads_.push_back(t);  // 将对象保存到 vector 中
		loops_.push_back(t->startLoop());  // 启动线程并获取其 eventloop 对象
	}
}

// 获取下一个要使用的 eventloop
eventloop *eventloopThreadPool::getNextLoop() {
	baseLoop_->assertInLoopThread();  // 确保在 baseLoop 所在的线程中调用
	assert(started_);  // 确保线程池已经启动
	eventloop *loop = baseLoop_;
	if (!loops_.empty()) {
		loop = loops_[next_];  // 获取下一个 eventloop
		next_ = (next_ + 1) % numThreads_;  // 更新下一个要使用的索引
	}
	return loop;
}
}