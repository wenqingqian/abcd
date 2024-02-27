#include "epoll.hpp"
namespace abcd{
	
epoll::epoll()
	:	maxfd_(conf.server.maxfd),
		epollfd_(epoll_create1(EPOLL_CLOEXEC)),
		events_(100)
	{}

void epoll::add(channelSp chan){
	int fd = chan->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	event.events = chan->getEvents();

	channels_[fd] = chan;

	if(epoll_ctl(epollfd_, EPOLL_CTL_ADD, fd, &event) < 0){
		channels_[fd].reset();
		panic("error epoll add");
	}
}
void epoll::mod(channelSp chan){
	int fd = chan->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	event.events = chan->getEvents();

	if(epoll_ctl(epollfd_, EPOLL_CTL_MOD, fd, &event) < 0){
		channels_[fd].reset();
		panic("error epoll mod");
	}
}

void epoll::del(channelSp chan){
	int fd = chan->getFd();
	struct epoll_event event;
	event.data.fd = fd;
	event.events = chan->getEvents();

	if (epoll_ctl(epollfd_, EPOLL_CTL_DEL, fd, &event) < 0) {
		channels_[fd].reset();
		panic("error epoll del");
	}

}

channelList epoll::poll(){
	int event_count = epoll_wait(epollfd_, &*events_.begin(), events_.size(), -1);
	if (event_count <= 0)
		panic(0);
	
	channelSpList epollget;
	for(int i = 0; i < event_count; ++i) {
		int fd = events_[i].data.fd;

		channelSp chan = channels_[fd];
		if(!chan){
			panic("invalid");
			continue;
		}
		chan->setRevents(events_[i].events);
		epollget.emplace_back(chan);
	}
	// rvo 
	// return std::move(epollget);
	return epollget;
}

} // namespace abcd
