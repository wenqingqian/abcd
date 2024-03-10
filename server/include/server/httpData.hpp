// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <sys/epoll.h>
#include <unistd.h>
#include <functional>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include "util.hpp"
namespace abcd{

class eventloop;
class channel;
using channelSp = std::shared_ptr<channel>;

class httpData : public std::enable_shared_from_this<httpData> {
public:
	httpData(eventloop *loop, int connfd);
	~httpData() { close(fd_); }
	void reset();

	channelSp getChannel() { return channel_; }
	eventloop *getLoop() { return loop_; }
	
	void handleClose();
	void newEvent();

private:
	eventloop *loop_;
	channelSp channel_;
	int fd_;
	std::string inBuffer_;
	std::string outBuffer_;
	bool error_;

	std::string fileName_;
	std::string path_;
	int nowReadPos_;

	bool keepAlive_;
	std::map<std::string, std::string> headers_;

	void handleRead();
	void handleWrite();
	void handleConn();
	void handleError(int fd, int err_num, std::string short_msg);

};

using httpDataSp = std::shared_ptr<httpData>;
}// namespace