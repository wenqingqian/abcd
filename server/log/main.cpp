#include "Logger.hpp"
#include "currentThread.hpp"
#include <iostream>
#include <thread>
#include <vector>
using namespace tsuki::util;
using namespace std;

void threadFunc(string name)
{
	currentThread::this_thread_init_once();
	currentThread::setname(name);
    // for (int i = 0; i < 100000; ++i)
    // {
    //     LOG << i;
    // }
	
	for (int i = 0; i < 100; ++i)
    {
        LOG_WARN << i;
    }
}

void type_test()
{
    // 13 lines
    cout << "----------type test-----------" << endl;
    LOG_DEBUG << 0;
    LOG_ERROR << 1234567890123;
    LOG_FATAL << 1.0f;
    LOG << 3.1415926;
    LOG << (short) 1;
    LOG << (long long) 1;
    LOG << (unsigned int) 1;
    LOG << (unsigned long) 1;
    LOG << (long double) 1.6555556;
    LOG << (unsigned long long) 1;
    LOG << 'c';
    LOG << "abcdefg";
    LOG << string("This is a string");
}

void stressing_single_thread()
{
    // 100000 lines
    cout << "----------stressing test single thread-----------" << endl;
    for (int i = 0; i < 1000; ++i)
    {
        LOG << i;
    }
}

void stressing_multi_threads(int threadNum = 4)
{
    // threadNum * 100000 lines
	vector v = {"t1","t2","t3","t4"};
    cout << "----------stressing test multi thread-----------" << endl;
    vector<thread> vsp;
    for (int i = 0; i < threadNum; ++i)
    {
       vsp.emplace_back(threadFunc, v[i]);
    }
    // for (int i = 0; i < threadNum; ++i)
    // {
		
    //     vsp[i]->start();
    // }
    
	for(auto& t:vsp){
		t.detach();
	}
}

void other()
{
    // 1 line
    cout << "----------other test-----------" << endl;
    LOG << "fddsa" << 'c' << 0 << 3.666 << string("This is a string");
}


int main()
{
    // 共500014行
    type_test();
    sleep(3);

    // stressing_single_thread();
    // sleep(3);

    other();
    sleep(3);

    stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(2);
	stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(2);
	stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(2);
	stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(2);
	stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(2);
	stressing_multi_threads();
	LOG_FATAL << "end";
	sleep(5);
	LOG<<"end";
    while(true);

	// LOG_ERROR << 1111;
    return 0;
}