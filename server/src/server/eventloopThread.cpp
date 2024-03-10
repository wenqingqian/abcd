// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include "eventloopThread.hpp"
#include <functional>

// 这个类的目的是在单独的线程中运行 eventloop，并提供一个接口 startLoop 返回对应的 eventloop 对象指针。
// 在析构函数中，会通知 eventloop 退出事件循环并等待线程退出。
// 在线程函数中，创建一个 eventloop 对象，然后运行其事件循环。

namespace abcd{

// eventloopThread 构造函数
eventloopThread::eventloopThread()
	:	loop_(nullptr),
		exiting_(false),
		thread_(std::bind(&eventloopThread::threadFunc, this), "eventT "),
		mutex_(),
		cond_() 
	{}

// eventloopThread 析构函数
eventloopThread::~eventloopThread() {
	exiting_ = true;
	if (loop_ != nullptr) {
		loop_->quit();  // 通知 eventloop 退出事件循环
		thread_.join();  // 等待线程退出
	}
}

// 启动事件循环线程并返回对应的 eventloop 对象指针
eventloop* eventloopThread::startLoop() {
	assert(!thread_.started());
	thread_.start();  // 启动线程

	{
		std::unique_lock<std::mutex> ul(mutex_);
		cond_.wait_for_seconds(ul,2);
	}

	LOG_INFO << "thread loop start";
	return loop_;
}

// 线程函数
void eventloopThread::threadFunc() {
	assert(currentThread::t_id_cache_ > 0);
	eventloop loop;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		loop_ = &loop;  // 设置 eventloop 对象指针
		cond_.notify();  // 通知启动线程的线程，eventloop 对象已经准备好
	}

	loop.loop();  // 运行事件循环
	// assert(exiting_);
	loop_ = nullptr;
}

}// namespace
