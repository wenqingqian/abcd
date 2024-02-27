#pragma once
#include "currentThread.hpp"
#include "lock.hpp"
#include "epoll.hpp"
#include "channel.hpp"
namespace abcd{

class eventloop : public noncopyable{
public:
	using functor     = std::function<void()>;
	using functorList = std::vector<functor>;

	eventloop();
	~eventloop(){}

	void loop();
	void quit();

	void runInLoop(functor);
	void queueInLoop(functor);
	bool isInLoopThread() const { return threadid_ == currentThread::tid(); }

	void del(channelSp chan) { poller_->del(chan); }
	void mod(channelSp chan) { poller_->mod(chan); }
	void add(channelSp chan) { poller_->add(chan); }

private:

	std::atomic<bool> looping_;
	std::atomic<bool> quit_;
	std::atomic<bool> event_handling_;
	std::atomic<bool> calling_pending_functors_;
	
	std::mutex mu_;
	epollUp poller_;
	int wakeupfd_;
	pid_t threadid_;

	functorList pending_functors_;
	channelSp wakeup_channel_;

	void wakeup();

	void doPendingFunctors();

	void handleRead();
};
}