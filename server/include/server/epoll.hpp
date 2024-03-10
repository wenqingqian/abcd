// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <memory>
#include <unordered_map>
#include <vector>
#include "channel.hpp"
#include "httpData.hpp"
namespace abcd{

class epoll {
public:
	epoll();
	~epoll(){}
	
	void add(channelSp request);
	void mod(channelSp request);
	void del(channelSp request);

	channelSpVec poll();

	int getepollFd() { return epollFd_; }

private:
	static const int MAXFDS = 100000;
	int epollFd_;
	std::vector<epoll_event> events_;
	channelSp fd2chan_[MAXFDS];

};

}// namespace