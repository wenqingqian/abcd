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

}