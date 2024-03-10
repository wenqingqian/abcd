// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <iostream>
#include <cstring>
#include "channel.hpp"
#include "eventloop.hpp"
#include "httpData.hpp"
namespace abcd{
#define to_string(x) std::__cxx11::to_string(x)

const uint32_t DEFAULT_EVENT = EPOLLIN | EPOLLET | EPOLLONESHOT;

httpData::httpData(eventloop *loop, int connfd)
	:	loop_(loop),
		channel_(new channel(connfd)),
		fd_(connfd),
		error_(false),
		nowReadPos_(0),
		keepAlive_(false) {
	// loop_->queueInLoop(bind(&httpData::setHandlers, this));
	channel_->setReadHandler(bind(&httpData::handleRead, this));
	channel_->setWriteHandler(bind(&httpData::handleWrite, this));
	channel_->setConnHandler(bind(&httpData::handleConn, this));
}

void httpData::reset() {
	// inBuffer_.clear();
	fileName_.clear();
	path_.clear();
	nowReadPos_ = 0;
	headers_.clear();
}

void httpData::handleRead() {
	std::string body_buff, header_buff;
	body_buff += "<html><title>hello</title>";
	body_buff += "<body bgcolor=\"ffffff\">";
	body_buff += "hello world";

	header_buff += "HTTP/1.1 200 ok\r\n";
	header_buff += "Content-Type: text/html\r\n";
	header_buff += "Connection: Close\r\n";
	header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";

	header_buff += "\r\n";
	outBuffer_ = header_buff + body_buff;
	handleWrite();
	channel_->setEvents(channel_->getEvents()|EPOLLIN);
}

void httpData::handleWrite() {

	uint32_t &events_ = channel_->getEvents();
	if (writen(fd_, outBuffer_) < 0) {
		perror("writen");
		events_ = 0;
		error_ = true;
	}
	if (outBuffer_.size() > 0) events_ |= EPOLLOUT;

}

void httpData::handleConn() {
	uint32_t &events_ = channel_->getEvents();
	if (events_ != 0) {
		if ((events_ & EPOLLIN) && (events_ & EPOLLOUT)) {
			events_ = 0;
			events_ |= EPOLLOUT;
		}
		events_ |= EPOLLET;
		loop_->mod(channel_);

	} else if (keepAlive_) {
		events_ |= (EPOLLIN | EPOLLET);
		loop_->mod(channel_);
	} else {
		events_ |= (EPOLLIN | EPOLLET);
		loop_->mod(channel_);
	}
	if (events_ & EPOLLOUT) {
		events_ = (EPOLLOUT | EPOLLET);
	} else {
		loop_->runInLoop(std::bind(&httpData::handleClose, shared_from_this()));
	}
}

void httpData::handleError(int fd, int err_num, string short_msg) {
	short_msg = " " + short_msg;
	char send_buff[4096];
	string body_buff, header_buff;
	body_buff += "<html><title>hello</title>";
	body_buff += "<body bgcolor=\"ffffff\">";

	header_buff += "HTTP/1.1 " + to_string(err_num) + short_msg + "\r\n";
	header_buff += "Content-Type: text/html\r\n";
	header_buff += "Connection: Close\r\n";
	header_buff += "Content-Length: " + to_string(body_buff.size()) + "\r\n";

	header_buff += "\r\n";
	// 错误处理不考虑writen不完的情况
	sprintf(send_buff, "%s", header_buff.c_str());
	writen(fd, send_buff, strlen(send_buff));
	sprintf(send_buff, "%s", body_buff.c_str());
	writen(fd, send_buff, strlen(send_buff));
}

void httpData::handleClose() {
	httpDataSp guard(shared_from_this());
	loop_->del(channel_);
}

void httpData::newEvent() {
	channel_->setEvents(DEFAULT_EVENT);
	loop_->add(channel_);
	handleRead();
}


}// namespace