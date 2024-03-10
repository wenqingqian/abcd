// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <functional>
#include "server.hpp"

namespace abcd{

// 构造函数，初始化 server 对象
server::server(eventloop *loop)
	:	loop_(loop),
		threadNum_(conf.server.thread_num),
		eventloopThreadPool_(new eventloopThreadPool(loop_, threadNum_)),
		started_(false),
		port_(conf.server.port),
		listenFd_(socket_bind_listen(port_)),
		acceptChannel_(new channel(listenFd_))
	{
	handle_for_sigpipe();  // 处理 SIGPIPE 信号
	if (setSocketNonBlocking(listenFd_) < 0) {  // 设置监听 socket 为非阻塞模式
		perror("set socket non block failed");
		abort();
	}
}

// 启动服务器
void server::start() {
	eventloopThreadPool_->start();  // 启动线程池
	LOG_INFO << "threadpool start";
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);  // 设置监听事件为可读和边缘触发模式
	acceptChannel_->setReadHandler(std::bind(&server::handNewConn, this));  // 设置可读事件的回调函数
	acceptChannel_->setConnHandler(std::bind(&server::handThisConn, this));  // 设置连接建立事件的回调函数
	loop_->add(acceptChannel_);  // 将监听通道加入 eventloop 的事件循环中
	started_ = true;  // 标记服务器已启动
	LOG_INFO << "server start";
}

// 处理新连接事件
void server::handNewConn() {
	sockaddr_in client_addr;
	memset(&client_addr, 0, sizeof(sockaddr_in));
	socklen_t client_addr_len = sizeof(client_addr);
	int accept_fd = 0;
	while ((accept_fd = accept(listenFd_, (sockaddr *)&client_addr,
								&client_addr_len)) > 0) {
		eventloop *loop = eventloopThreadPool_->getNextLoop();  // 获取下一个 eventloop
		LOG << "New connection from " << inet_ntoa(client_addr.sin_addr) << ":"
			<< ntohs(client_addr.sin_port);

		// 以下为对新连接的一些处理，如设定非阻塞、限制最大并发连接数等

		if (accept_fd >= MAXFDS) {
			close(accept_fd);
			continue;
		}

		if (setSocketNonBlocking(accept_fd) < 0) {
			LOG << "Set non block failed!";
			return;
		}

		setSocketNodelay(accept_fd);

		httpDataSp req_info(new httpData(loop, accept_fd));
		req_info->getChannel()->setHolder(req_info);
		loop->queueInLoop(std::bind(&httpData::newEvent, req_info));
	}
	acceptChannel_->setEvents(EPOLLIN | EPOLLET);  // 重新设置监听事件
}

}// namespace