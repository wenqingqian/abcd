#pragma once
#include "eventloop.hpp"

namespace abcd{

class channel : public noncopyable
{
public:
	using Callback = std::function<void()>;

	channel(eventloop* loop, int fd);
	~channel(){}

	void handleEvent();
	void setReadCallback(Callback cb)
	{ read_callback_ = std::move(cb); }
	void setWriteCallback(Callback cb)
	{ write_callback_ = std::move(cb); }
	void setCloseCallback(Callback cb)
	{ close_callback_ = std::move(cb); }
	void setErrorCallback(Callback cb)
	{ error_callback_ = std::move(cb); }


	int getFd() const { return fd_; }
	int getEvents() const { return events_; }
	int getRevents() const { return revents_; }
	void setRevents(int revt) { revents_ = revt; }

	eventloop* ownerLoop() { return loop_; }

private:

	void handleEvent();


	eventloop* loop_;
	const int  fd_;
	int        events_;
	int        revents_; // it's the received event types of epoll or poll


	Callback readCallback_;
	Callback writeCallback_;
	Callback closeCallback_;
	Callback errorCallback_;
};

}