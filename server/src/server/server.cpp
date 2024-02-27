#include "server.hpp"
#include "socket.hpp"

namespace abcd{

server::server()
	:	port_(8002),
		listen_fd_(socketBindListen(port_))
	{
	addsig(SIGPIPE, SIG_IGN);

}

void server::start(){

}

}