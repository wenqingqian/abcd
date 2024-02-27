#pragma once
#include "channel.hpp"
#include "socket.hpp"
namespace abcd{

class epoll{
public:
	epoll();
	~epoll(){}
	void add(channelSp);
	void mod(channelSp);
	void del(channelSp);
	channelSpList poll();
	int getEpollFd() { return epollfd_; }

private:
	int maxfd_;
	int epollfd_;
	std::vector<epoll_event> events_;
	channelSpMap channels_;
};

using epollSp = std::shared_ptr<epoll>;
using epollUp = std::unique_ptr<epoll>;
}
