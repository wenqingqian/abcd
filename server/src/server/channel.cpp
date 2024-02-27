#include "channel.hpp"
#include <sys/epoll.h>
#include "common.hpp"
namespace abcd{

channel::channel(eventloop* loop, int fd)
	:	loop_(loop),
		fd_(fd),
		events_(0),
		revents_(0)
	{}

void channel::handleEvent()
{
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
	{
		if (closeCallback_) closeCallback_();
	}
	if (revents_ & (EPOLLERR))
	{
		if (errorCallback_) errorCallback_();
	}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (readCallback_) readCallback_();
	}
	if (revents_ & EPOLLOUT)
	{
		if (writeCallback_) writeCallback_();
	}

}



}