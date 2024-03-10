#include "common.hpp"
#include "server.hpp"
#include <iostream>
#include "TomlConfig.hpp"

using namespace abcd;

__attribute__((constructor)) void start_(){ 
	// setenv("TZ", "America/New_York", 1);
	conf.init("server.toml"); 
}

namespace abcd{extern void stopAsyncLogger();}
__attribute__((destructor)) void end_(){
	stopAsyncLogger();
}

int main(int argc, char* argv[]){

	currentThread::this_thread_init_once();
	
	LOG_INFO << "muduo start";
	eventloop mainloop;
	server httpserver(&mainloop);
	httpserver.start();
	mainloop.loop();

}
