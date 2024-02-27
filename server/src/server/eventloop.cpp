#include "eventloop.hpp"

#include "socket.hpp"
#include <sys/eventfd.h>
namespace abcd{

thread_local eventloop* t_loopInThisThread = nullptr;

int createEventfd(){
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if (evtfd < 0){
		panic("error eventfd");
	}
	return evtfd;
}

eventloop::eventloop()
	:	looping_(false),
		quit_(false),
		event_handling_(false),
		calling_pending_functors_(false),

		threadid_(currentThread::tid()),
		poller_(new epoll()),
		
		wakeupfd_(createEventfd()),
		wakeup_channel_(new channel(wakeupfd_))
	{
	LOG_DEBUG << "eventloop created " << this << " in thread " << threadid_;
	if (t_loopInThisThread){
		LOG_FATAL << "Another eventloop " << t_loopInThisThread
				<< " exists in this thread " << threadid_;
	}else{
		t_loopInThisThread = this;
	}
	wakeup_channel_->setReadCallback(std::bind(&eventloop::handleRead), this);
	wakeup_channel_->setEvents(EPOLLIN | EPOLLET);
	poller_->add(wakeup_channel_);
}

void eventloop::wakeup() {
	uint64_t one = 1;
	ssize_t n = writen(wakeupfd_, (char*)(&one), sizeof one);
	if (n != sizeof one) {
		LOG_ERROR << "eventloop::wakeup() writes " << n << " bytes instead of 8";
	}
}

// 处理读事件的回调函数
void eventloop::handleRead() {
	uint64_t one = 1;
	ssize_t n = readn(wakeupfd_, &one, sizeof one);
	if (n != sizeof one) {
		LOG_ERROR << "eventloop::handleRead() reads " << n << " bytes instead of 8";
	}
	wakeup_channel_->setEvents(EPOLLIN | EPOLLET);
}

void eventloop::doPendingFunctors(){
	functorList functors;
	calling_pending_functors_ = true;
	{
		std::lock_guard<std::mutex> lock(mu_);
		functors.swap(pending_functors_);
	}

	for (const functor& functor : functors){
		functor();
	}

	calling_pending_functors_ = false;
}
void eventloop::quit() {
	quit_ = true;
	if (!isInLoopThread()) {
		wakeup();
	}
}

// 运行指定任务在事件循环线程
void eventloop::runInLoop(functor cb) {
	if (isInLoopThread())
		cb();
	else
		queueInLoop(std::move(cb));
}

// 将任务加入队列
void eventloop::queueInLoop(functor cb) {
	{
		std::lock_guard<std::mutex> lock(mu_);
		pending_functors_.emplace_back(std::move(cb));
	}
	if (!isInLoopThread() || calling_pending_functors_) wakeup();
}

// 事件循环主函数
void eventloop::loop() {
	assert(isInLoopThread());
	looping_ = true;
	quit_ = false;
	channelSpList ret;
	while (!quit_) {
		ret.clear();
		ret = poller_->poll();
		event_handling_ = true;
		for (auto& it : ret)
			it->handleEvents();  // 处理事件
		event_handling_ = false;
		doPendingFunctors();  // 处理任务队列中的任务
	}
	looping_ = false;
}

} // namepace abcd