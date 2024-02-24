#include "asyncLogger.hpp"
#include "logger.hpp"
#include "config.hpp"
namespace abcd{

asyncLogger::asyncLogger()
	: 	mu_(),
		latch_(),
		running_(true),
		cond_()
	{
	current_bufferptr_ = std::make_shared<buffer>();
	flush_val_ 	 = conf.log.flush_val;
	fullsize_  	 = conf.log.buffer_vector_size;
	freesize_  	 = conf.log.buffer_vector_free_size;
	maxfullsize_ = conf.log.buffer_to_write_max;
	freequeue_ 	 = buffervec(freesize_, std::make_shared<buffer>());

	current_bufferptr_->bzero();
	fullqueue_.reserve(fullsize_);
	for (auto &&bf : freequeue_){
		bf->bzero();
	}
}

void asyncLogger::append(logStream &ls){
	std::unique_lock<std::mutex> lg(mu_);
	if (current_bufferptr_->avail() > ls.length()){
		current_bufferptr_->append(ls.data(), ls.length());
		if(ls.level_ >= logLevel::FATAL){
			cond_.notify_all();
		}
	}else{
		fullqueue_.push_back(current_bufferptr_);
		current_bufferptr_.reset();
		if (freequeue_.empty()){
			current_bufferptr_ = std::make_shared<buffer>();
		}
		else{
			current_bufferptr_ = std::move(freequeue_.back());
			freequeue_.pop_back();
		}
		current_bufferptr_->append(ls.data(), ls.length());
		cond_.notify_all();
	}
}
void asyncLogger::asyncLogFunc(){
	latch_.count_down();
	buffervec freequeueBackup = buffervec(freesize_, std::make_shared<buffer>());
	for (auto &&v : freequeueBackup){
		v->bzero();
	}
	
	buffervec bufferToWrite;
	bufferToWrite.reserve(fullsize_);
	logFile file_;
	while (running_){
		{
			std::unique_lock<std::mutex> lg(mu_);
			if (fullqueue_.empty()){
				cond_.wait_for_seconds(lg, flush_val_);
			}
			fullqueue_.push_back(current_bufferptr_);
			current_bufferptr_.reset();

			bufferToWrite.swap(fullqueue_);
			current_bufferptr_ = std::move(freequeueBackup.back());
			freequeueBackup.pop_back();

			freequeue_.swap(freequeueBackup);
		}
		//处理过大的日志缓存
		if( bufferToWrite.size() > maxfullsize_ ){
			bufferToWrite.erase(bufferToWrite.begin()+2,bufferToWrite.end());
		}
		bool is_need_flush = false;
		//输出到file
		for(auto&& buf : bufferToWrite){
			if(buf->length()!=0) is_need_flush = true;
			file_.append(buf->data(),buf->length());
		}
		//准备新的freequeue
		int idx = freesize_-freequeueBackup.size();
		for(auto&& buf : bufferToWrite){
			if(--idx <= 0){ break; }
			buf->reset();
			freequeueBackup.push_back(std::move(buf));
		}
		idx = freesize_-freequeueBackup.size();
		while( idx>0 ){
			freequeueBackup.emplace_back(std::make_shared<buffer>());
			idx--;
		}

		bufferToWrite.clear();
		if(bufferToWrite.capacity() > 2*fullsize_){
			bufferToWrite.reserve(fullsize_);
		}

		if(is_need_flush) file_.flush();
	}
	file_.flush();
}

} // namespace abcd
