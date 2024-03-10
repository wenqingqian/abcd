#pragma once
#include <memory>
#include "channel.hpp"
#include "eventloop.hpp"
#include "eventloopThreadPool.hpp"
namespace abcd{

class server {
public:
	server(eventloop *loop);
	~server() {}
	eventloop *getLoop() const { return loop_; }
	void start();
	void handNewConn();
	void handThisConn() { loop_->mod(acceptChannel_); }

private:
	eventloop *loop_;
	int threadNum_;
	std::unique_ptr<eventloopThreadPool> eventloopThreadPool_;
	bool started_;

	int port_;
	int listenFd_;
	channelSp acceptChannel_;
	static const int MAXFDS = 100000;
};

}