// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <functional>
#include <memory>
#include <string>
#include <unordered_map>
#include "httpData.hpp"
namespace abcd{

class eventloop;

class channel {
private:
	using callback = std::function<void()>;

	int fd_;
	uint32_t events_;
	uint32_t revents_;
	uint32_t lastEvents_;

	// 方便找到上层持有该channel的对象
	std::weak_ptr<httpData> holder_;

private:

	callback readHandler_ ;
	callback writeHandler_;
	callback errorHandler_;
	callback connHandler_ ;

public:
	channel(){}
	channel(int fd) 
		:fd_(fd), events_(0), lastEvents_(0){}
	~channel(){}

	int getFd(){ return fd_; }
	void setFd(int fd) { fd_ = fd; }

	void setHolder(httpDataSp holder) { holder_ = holder; }
	httpDataSp getHolder() {
		httpDataSp ret(holder_.lock());
		return ret;
	}

	void setReadHandler (callback &&readHandler ) {readHandler_  = readHandler ;}
	void setWriteHandler(callback &&writeHandler) {writeHandler_ = writeHandler;}
	void setErrorHandler(callback &&errorHandler) {errorHandler_ = errorHandler;}
	void setConnHandler (callback &&connHandler ) {connHandler_  = connHandler ;}

	void handleRead() {	if (readHandler_ ) {readHandler_() ;}};
	void handleWrite(){	if (writeHandler_) {writeHandler_();}};
	void handleConn() {	if (connHandler_ ) {connHandler_() ;}};

	void setRevents(uint32_t ev) { revents_ = ev; }
	void setEvents(uint32_t ev) { events_ = ev; }
	uint32_t &getEvents() { return events_; }

	bool EqualAndUpdateLastEvents() {
		bool ret = (lastEvents_ == events_);
		lastEvents_ = events_;
		return ret;
	}

	uint32_t getLastEvents() { return lastEvents_; }

		void handleEvents() {
		events_ = 0;
		if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
			events_ = 0;
			return;
		}
		if (revents_ & EPOLLERR) {
		if (errorHandler_) errorHandler_();
			events_ = 0;
			return;
		}
		if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
			handleRead();
		}
		if (revents_ & EPOLLOUT) {
			handleWrite();
		}
		handleConn();
	}
};

using channelSp = std::shared_ptr<channel>;
using channelSpVec = std::vector<channelSp>;

} // namespace