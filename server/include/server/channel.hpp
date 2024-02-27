#pragma once
#include "eventloop.hpp"
#include <functional>
#include <memory>
#include <map>
#include <vector>

namespace abcd{

class channel : public noncopyable
{
public:
	using callback = std::function<void()>;

	channel(int fd);
	~channel(){}

	void handleEvent();
	void setReadCallback(callback cb)
	{ read_callback_ = std::move(cb);  }
	void setWriteCallback(callback cb)
	{ write_callback_ = std::move(cb); }
	void setCloseCallback(callback cb)
	{ close_callback_ = std::move(cb); }
	void setErrorCallback(callback cb)
	{ error_callback_ = std::move(cb); }


	int  getFd() const { return fd_; }
	int  getEvents () const   { return events_;  }
	int  getRevents() const   { return revents_; }
	void setEvents (int evt ) { events_ = evt;   }
	void setRevents(int revt) { revents_ = revt; }

private:
	const int  fd_;
	int        events_;
	int        revents_; // it's the received event types of epoll or poll


	callback read_callback_;
	callback write_callback_;
	callback close_callback_;
	callback error_callback_;
};

using channelSp     = std::shared_ptr<channel>; 
using channelUp     = std::unique_ptr<channel>;
using channelSpList = std::vector<channelSp>;
using channelSpMap  = std::map<int, channelSp>;
}