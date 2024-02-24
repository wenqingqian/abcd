#include "../include/SyncUtil.hpp"
#include<iostream>
#include<type_traits>
#include<memory>
#include<span>
// #include "memory/shared_ptr.h"
#include<thread>
#include<cstring>
#include<mutex>
#include<vector>
using namespace std;
using namespace tsuki::util;

int main(){
	//!Condition test
	Condition cd;
	mutex mu_;
	using namespace std::chrono_literals;
	thread t1 = thread([&](){
		this_thread::sleep_for(3s);
		cd.notify_one();
	});
	thread t2 = thread([&](){
		unique_lock<mutex> lg(mu_);
		cd.wait_for_seconds(lg,10);
		cout<<1<<endl;
	});
	thread t3 = thread([&](){
		this_thread::sleep_for(1s);
		unique_lock<mutex>lg(mu_);
		cout<<3<<endl;
	});
	t1.join();
	t2.join();
	t3.join();
}