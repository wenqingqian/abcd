// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <iostream>
#include "eventloop.hpp"
namespace abcd{

thread_local eventloop* t_loopInThisThread = 0;
// 创建一个 eventfd 用于事件唤醒
int createEventfd() {
	int evtfd = eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0) {
		LOG << "Failed in eventfd";
		abort();
	}
	return evtfd;
}

// eventloop 构造函数
eventloop::eventloop()
	:	looping_(false),
		poller_(new epoll()),  // 使用 Epoll 作为事件轮询器
		wakeupFd_(createEventfd()),  // 创建用于唤醒的 eventfd
		quit_(false),
		eventHandling_(false),
		callingPendingFunctors_(false),
		threadId_(currentThread::t_id_cache_),
		pwakeupChannel_(new channel(wakeupFd_))
	{
	// 将当前线程的 eventloop 对象保存在线程局部变量中
	if (t_loopInThisThread) {
		// LOG << "Another eventloop " << t_loopInThisThread << " exists in this thread " << threadId_;
	} else {
		t_loopInThisThread = this;
	}
	// 设置唤醒事件通道的事件类型和回调函数
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
	pwakeupChannel_->setReadHandler(std::bind(&eventloop::handleRead, this));
	pwakeupChannel_->setConnHandler(std::bind(&eventloop::handleConn, this));
	poller_->add(pwakeupChannel_);
}

// 处理连接事件的回调函数
void eventloop::handleConn() {
	poller_->mod(pwakeupChannel_);
}

// 析构函数，释放资源
eventloop::~eventloop() {
	close(wakeupFd_);
	t_loopInThisThread = nullptr;
}

// 事件唤醒
void eventloop::wakeup() {
	uint64_t one = 1;
	ssize_t n = writen(wakeupFd_, (char*)(&one), sizeof one);
	if (n != sizeof one) {
		LOG << "eventloop::wakeup() writes " << n << " bytes instead of 8";
	}
}

// 处理读事件的回调函数
void eventloop::handleRead() {
	uint64_t one = 1;
	ssize_t n = readn(wakeupFd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG << "eventloop::handleRead() reads " << n << " bytes instead of 8";
	}
	pwakeupChannel_->setEvents(EPOLLIN | EPOLLET);
}

// 运行指定任务在事件循环线程
void eventloop::runInLoop(functor&& cb) {
	if (isInLoopThread())
		cb();
	else
		queueInLoop(std::move(cb));
}

// 将任务加入队列
void eventloop::queueInLoop(functor&& cb) {
	{
		std::lock_guard<std::mutex> lock(mutex_);
		pendingFunctors_.emplace_back(std::move(cb));
	}

	if (!isInLoopThread() || callingPendingFunctors_) wakeup();
}

// 事件循环主函数
void eventloop::loop() {
	assert(!looping_);
	assert(isInLoopThread());
	looping_ = true;
	quit_ = false;
	channelSpVec ret;
	while (!quit_) {
		ret.clear();
		ret = poller_->poll();
		eventHandling_ = true;
		for (auto& it : ret) it->handleEvents();  // 处理事件
		eventHandling_ = false;
		doPendingFunctors();  // 处理任务队列中的任务
	}
	looping_ = false;
}

// 处理任务队列中的任务
void eventloop::doPendingFunctors() {
	std::vector<functor> functors;
	callingPendingFunctors_ = true;

	{
		std::lock_guard<std::mutex> lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for (size_t i = 0; i < functors.size(); ++i) functors[i]();
	callingPendingFunctors_ = false;
}

// 退出事件循环
void eventloop::quit() {
	quit_ = true;
	if (!isInLoopThread()) {
		wakeup();
	}
}

}//namespace