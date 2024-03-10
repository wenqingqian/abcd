// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <assert.h>
#include <errno.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <deque>
#include <queue>
#include <arpa/inet.h>
#include <iostream>
#include "epoll.hpp"
#include "common.hpp"
namespace abcd{

const int EVENTSNUM = 4096;
const int EPOLLWAIT_TIME = 10000;

epoll::epoll()
	:epollFd_(epoll_create1(EPOLL_CLOEXEC)), events_(EVENTSNUM){}

void epoll::add(channelSp request) {
	int fd = request->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	event.events = request->getEvents();

	request->EqualAndUpdateLastEvents();

	fd2chan_[fd] = request;
	if (epoll_ctl(epollFd_, EPOLL_CTL_ADD, fd, &event) < 0) {
		perror("epoll_add error");
		fd2chan_[fd].reset();
	}
}

void epoll::mod(channelSp request) {
	int fd = request->getFd();
	if (!request->EqualAndUpdateLastEvents()) {
		struct epoll_event event;
		event.data.fd = fd;
		event.events = request->getEvents();
		if (epoll_ctl(epollFd_, EPOLL_CTL_MOD, fd, &event) < 0) {
			LOG_ERROR << "epoll_mod error";
			fd2chan_[fd].reset();
		}
	}
}

void epoll::del(channelSp request) {
	int fd = request->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	event.events = request->getLastEvents();
	if (epoll_ctl(epollFd_, EPOLL_CTL_DEL, fd, &event) < 0) {
		LOG_ERROR << "epoll_del error";
	}
	fd2chan_[fd].reset();
}

channelSpVec epoll::poll() {
	while (true) {
		int event_count =
			epoll_wait(epollFd_, &*events_.begin(), events_.size(), EPOLLWAIT_TIME);
		if (event_count < 0) LOG_ERROR << "epoll wait error";
		channelSpVec req_data;
		for (int i = 0; i < event_count; ++i) {
			int fd = events_[i].data.fd;
			channelSp cur_req = fd2chan_[fd];
			if (cur_req) {
				cur_req->setRevents(events_[i].events);
				cur_req->setEvents(0);
				req_data.push_back(cur_req);
			} else {
				LOG << "SP cur_req is invalid";
			}
		}
		if (req_data.size() > 0) return req_data;
	}
}

}//namespace