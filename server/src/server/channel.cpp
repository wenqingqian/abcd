#include "channel.hpp"
#include <sys/epoll.h>
#include "common.hpp"
namespace abcd{

channel::channel(int fd)
	:	fd_(fd),
		events_(0),
		revents_(0)
	{}

void channel::handleEvent()
{
	if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN))
	{
		if (close_callback_) close_callback_();
	}
	if (revents_ & (EPOLLERR))
	{
		if (error_callback_) error_callback_();
	}
	if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
	{
		if (read_callback_) read_callback_();
	}
	if (revents_ & EPOLLOUT)
	{
		if (write_callback_) write_callback_();
	}

}



}