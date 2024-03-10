// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include "channel.hpp"
#include "epoll.hpp"
#include "currentThread.hpp"
#include "common.hpp"
#include "util.hpp"
#include "thread.hpp"
namespace abcd{
	
class eventloop {
public:
	using functor = std::function<void()>;

	eventloop();
	~eventloop();
	void loop();
	void quit();
	void runInLoop(functor&& cb);
	void queueInLoop(functor&& cb);
	bool isInLoopThread() const { return threadId_ == currentThread::t_id_cache_; }
	void assertInLoopThread() { assert(isInLoopThread()); }
	
	void add(channelSp chan){poller_->add(chan);}
	void mod(channelSp chan){poller_->mod(chan);}
	void del(channelSp chan){poller_->del(chan);}

private:
	// 声明顺序 wakeupFd_ > pwakeupChannel_
	bool looping_;
	std::shared_ptr<epoll> poller_;
	int wakeupFd_;
	bool quit_;
	bool eventHandling_;
	mutable std::mutex mutex_;
	std::vector<functor> pendingFunctors_;
	bool callingPendingFunctors_;
	const pid_t threadId_;
	channelSp pwakeupChannel_;

	void wakeup();
	void handleRead();
	void doPendingFunctors();
	void handleConn();
};

}//namespace