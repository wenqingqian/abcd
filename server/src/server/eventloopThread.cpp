#include "eventloopThread.h"
#include <functional>

namespace abcd{
// 这个类的目的是在单独的线程中运行 eventloop，并提供一个接口 startLoop 返回对应的 eventloop 对象指针。
// 在析构函数中，会通知 eventloop 退出事件循环并等待线程退出。
// 在线程函数中，创建一个 eventloop 对象，然后运行其事件循环。

// eventloopThread 构造函数
eventloopThread::eventloopThread()
	:	loop_(NULL),
		exiting_(false),
		thread_(bind(&eventloopThread::threadFunc, this), "eventloopThread"),
		mutex_(),
		cond_(mutex_) 
	{}

// eventloopThread 析构函数
eventloopThread::~eventloopThread() {
	exiting_ = true;
	if (loop_ != NULL) {
		loop_->quit();  // 通知 eventloop 退出事件循环
		thread_.join();  // 等待线程退出
	}
}

// 启动事件循环线程并返回对应的 eventloop 对象指针
eventloop* eventloopThread::startLoop() {
	assert(!thread_.started());
	thread_.start();  // 启动线程
	{
		std::lock_guard<std::mutex> lock(mutex_);
		// 一直等到threadFun在Thread里真正跑起来
		while (loop_ == NULL) cond_.wait();  // 等待线程启动完成
	}
	return loop_;
}

// 线程函数
void eventloopThread::threadFunc() {
	eventloop loop;
	{
		std::lock_guard<std::mutex> lock(mutex_);
		loop_ = &loop;  // 设置 eventloop 对象指针
		cond_.notify();  // 通知启动线程的线程，eventloop 对象已经准备好
	}

	loop.loop();  // 运行事件循环
	// assert(exiting_);
	loop_ = NULL;
}

}