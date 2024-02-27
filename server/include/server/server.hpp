#pragma once
namespace abcd{

class server : public noncopyable{
public:
	server();

	void start();
private:
	int listen_fd_;
	int port_;
};

}