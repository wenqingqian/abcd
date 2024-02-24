#pragma once
#include "noncopyable.hpp"
#include <condition_variable>
#include <mutex>
#include <atomic>
#include <chrono>
#include <pthread.h>
// #include <barrier>
// #include <latch>
#include <assert.h>

namespace abcd{

// latch ~= std::latch ,没有改成新的std::latch
// 在异步日志中有使用, 可以换成std::latch(速度快一点)
class latch : public noncopyable{
public:
	// std::latch
	explicit latch(int v = 1):count_(v),flag_(false){}
	void count_down(){
		int old = count_.fetch_sub(1,std::memory_order_release);
		if(old == 1) {flag_=true;flag_.notify_all();}
		// if(old==1){count_.notify_all();}
	}
	void wait(){flag_.wait(false);}
	// void wait(){std::__atomic_wait_address(&count_,[this]{return this->try_wait();});}
	bool try_wait(){return count_.load(std::memory_order_acquire)==0;}
private:
	std::atomic<bool> flag_;
	std::atomic<int> count_;
};

//BinaryLatch AsyncNotify n notify to 1 wait
//spin for seconds
// class BinarySpinLatch : public noncopyable {
// public:
// 	BinarySpinLatch():latch_(ATOMIC_FLAG_INIT){}
// 	void release(){latch_.test_and_set(std::memory_order_release);}
// 	void acquire_for_seconds(int seconds){
// 		auto now = std::chrono::system_clock::now();
// 		auto end_time = now + std::chrono::seconds(seconds);
// 		while (!latch_.test(std::memory_order_acquire)) {
// 			// spin for seconds
// 			now = std::chrono::system_clock::now();
// 			if (now >= end_time) {
// 				break; 
// 			}
// 		}
// 		//clear dont care about its state
// 		latch_.clear();
// 	}
// private:
// 	std::atomic_flag latch_;
// };

class condition : public noncopyable{
public:
	explicit condition():flag_(ATOMIC_FLAG_INIT),cond_(){}
	void notify(){cond_.notify_all();}
	void notify_one(){flag_.test_and_set(std::memory_order_release);cond_.notify_one();}
	void notify_all(){flag_.test_and_set(std::memory_order_release);cond_.notify_all();}
	void wait(auto& ul_){cond_.wait(ul_,[this]{return flag_.test(std::memory_order_acquire);});flag_.clear();}
	bool wait_for_seconds(auto& ul_, int seconds){
		// std::unique_lock<std::mutex> lock(mu_);
		auto now = std::chrono::system_clock::now();
		auto duration = std::chrono::seconds(seconds);
		return cond_.wait_until(ul_, now + duration) == std::cv_status::timeout;
	}
private:
	std::condition_variable cond_;
	std::atomic_flag flag_;
};
}//namespace abcd