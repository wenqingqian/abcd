#include "socket.hpp"

namespace abcd{

void addsig(int sig, void( handler )(int)){
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = handler;
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}

int socketBindListen(int port){
	int listenfd = socket(PF_INET, SOCK_STREAM, 0);

	struct sockaddr_in address;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_family = AF_INET;
	address.sin_port = htons(port);

	// 端口复用
	int reuse = 1;
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof( reuse ));
	
	if(bind(listenfd, ( struct sockaddr* )&address, sizeof( address )) == -1
			|| listen(listenfd, 5) == -1
			|| listenfd == -1)
		{
		close(listenfd);
		LOG_FATAL << "bind_listen listenfd error";
		panic(0);
		return -1;
	}

	return listenfd;
}


ssize_t readn(int fd, void *buff, size_t n) {
	size_t n = n;
	ssize_t nread = 0;
	ssize_t readSum = 0;
	char *ptr = (char *)buff;
	while (n > 0) {
		if ((nread = read(fd, ptr, n)) < 0) {
			if (errno == EINTR)
				nread = 0;
			else if (errno == EAGAIN) {
				return readSum;
			} else {
				return -1;
			}
		} else if (nread == 0)
			break;
		readSum += nread;
		n -= nread;
		ptr += nread;
	}
	return readSum;
}

ssize_t writen(int fd, void *buff, size_t n) {
	ssize_t nwritten = 0;
	ssize_t writeSum = 0;
	char *ptr = (char *)buff;
	while (n > 0) {
		if ((nwritten = ::write(fd, ptr, n)) <= 0) {
			if (nwritten < 0) {
				if (errno == EINTR) {
					nwritten = 0;
					continue;
				} else if (errno == EAGAIN) {
					return writeSum;
				} else
					return -1;
			}
		}
		writeSum += nwritten;
		n -= nwritten;
		ptr += nwritten;
	}
	return writeSum;
}

}