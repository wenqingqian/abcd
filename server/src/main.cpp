#include "logger.hpp"
#include "common.hpp"
#include <getopt.h>
#include <iostream>
#include "TomlConfig.hpp"
#include "config.hpp"
#include <vector>
#include <thread>

using namespace abcd;
using namespace std;

__attribute__((constructor)) void start_(){ 
	// setenv("TZ", "America/New_York", 1);
	conf.init("server.toml"); 
}

namespace abcd{extern void stopAsyncLogger();}
__attribute__((destructor)) void end_(){
	stopAsyncLogger();
}

int main(int argc, char* argv[]){

	output(conf.log.level);
	output(conf.server.port);
	LOG_TEST << "test";
	
	mtime time;
	int x;
	for(int i=0;i<100000000;i++){
		x++;
	}
	mtime no;
	std::cout<<no.diff<mtime::millisecond>(time);

}
