#include "AsyncLogger.hpp"
#include "Logger.hpp"
namespace tsuki::util{

AsyncLogger::AsyncLogger()
	: 	mu_(),
		latch_(),
		running_(true),
		cond_(),
		configAsync(CONFIG["log"]["AsyncLogger"])
	{
	current_bufferptr_ = std::make_shared<buffer>();
	flush_val_ 	 = *(configAsync["flush_val"].value<int>());
	fullsize_  	 = *(configAsync["buffer_vector_size"].value<int>());
	freesize_  	 = *(configAsync["buffer_vector_free_size"].value<int>());
	maxfullsize_ = *(configAsync["buffer_to_write_max"].value<int>());
	freequeue_ 	 = buffervec(freesize_, std::make_shared<buffer>());

	current_bufferptr_->bzero();
	fullqueue_.reserve(fullsize_);
	for (auto &&bf : freequeue_){
		bf->bzero();
	}
}
void AsyncLogger::append(LogStream &ls){
	std::unique_lock<std::mutex> lg(mu_);
	// std::cout<<ls.data();
	// /*printf("%s%d\n",__FILE__,__LINE__);*/
	if (current_bufferptr_->avail() > ls.length()){
		// std::cout<<ls.data();
		// /*printf("%s%d\n",__FILE__,__LINE__);*/
		current_bufferptr_->append(ls.data(), ls.length());
		if(ls.level_ >= Logger::log_level::FATAL){
			cond_.notify_all();
		}
	}else{
		/*printf("%s%d\n",__FILE__,__LINE__);*/
		fullqueue_.push_back(current_bufferptr_);
		current_bufferptr_.reset();
		if (freequeue_.empty()){
			/*printf("%s%d\n",__FILE__,__LINE__);*/
			current_bufferptr_ = std::make_shared<buffer>();
		}
		else{
			/*printf("%s%d\n",__FILE__,__LINE__);*/
			current_bufferptr_ = std::move(freequeue_.back());
			freequeue_.pop_back();
		}
		current_bufferptr_->append(ls.data(), ls.length());
		cond_.notify_all();
	}
	/*printf("%s%d\n",__FILE__,__LINE__);*/
}
void AsyncLogger::asyncLogFunc(){
	std::cout<<"async start"<<std::endl;
	std::cout<<"async start"<<std::endl;
	// AsyncLogger* async = static_cast<AsyncLogger*>(a);
	/*printf("%s%d\n",__FILE__,__LINE__);*/
	latch_.count_down();
	/*printf("%s%d\n",__FILE__,__LINE__);*/
	buffervec freequeueBackup = buffervec(freesize_, std::make_shared<buffer>());
	for (auto &&v : freequeueBackup){
		v->bzero();
	}
	/*printf("%s%d\n",__FILE__,__LINE__);*/
	buffervec bufferToWrite;
	bufferToWrite.reserve(fullsize_);
	LogFile file_;
	while (running_){
		{
			/*printf("%s%d\n",__FILE__,__LINE__);*/
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

} // namespace tsuki::util
