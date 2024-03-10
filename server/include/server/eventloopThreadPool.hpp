// @Author Lin Ya
// @Email xxbbb@vip.qq.com
#pragma once
#include <memory>
#include <vector>
#include "eventloopThread.hpp"
#include "common.hpp"
namespace abcd{

class eventloopThreadPool : noncopyable {
public:
	eventloopThreadPool(eventloop* baseLoop, int numThreads);

	~eventloopThreadPool() { LOG << "~eventloopThreadPool()"; }
	void start();

	eventloop* getNextLoop();

private:
	eventloop* baseLoop_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::shared_ptr<eventloopThread>> threads_;
	std::vector<eventloop*> loops_;
};

}// namespace