#include "logFile.hpp"
#include "logger.hpp"
#include "config.hpp"
#include <assert.h>
#include <sstream>
#include <chrono>
namespace abcd{

logFile::logFile()
	:	time_(),
		file_path_(conf.log.output_path),
		output_(),
		roll_size_(conf.log.roll_size),
		total_size_max_(conf.log.log_store_limit),
		time_check_interval_(conf.log.time_check_interval),
		history_record_path_(conf.log.history_record_path)
	{

	//*检查当前log是否是空log,如果有历史信息就先提取历史时间
	std::ifstream ifs(file_path_);
	ifs.seekg(0, std::ios::end);
	if(ifs.tellg() != 0){
		std::string line;
		ifs.seekg(0,std::ios::beg);
		std::getline(ifs, line);
		time_.reset(line);
		time_init_ = false;
	}
	ifs.close();

	output_.reset(file_path_,std::ios::app);
	//! 修改不了ofstream的缓冲区, 目前来看, 如果这里收到了数据
	//* 1.asynclogger 的current buffer 已经满了并扔来了至少4MB数据
	//* 2.已经过了3秒
	//* 因此这里的缓冲区可能没有那么重要, 甚至对于async的主动flush也应该有所考量
	//? 或许可以在前面判断一下传递数据大致大小, 大数据不flush, 小数据flush.
	//* 这与muduo用FILE对缓冲区做更主动的控制有所区别
	// output_.rdbuf()->pubsetbuf(buffer_,sizeof(buffer_));
}

void logFile::append(const char* data, int length){
	//*每隔4小时判断下是否超过一天没rollLog
	if(--time_check_interval_ <=0){
		time_check_interval_ = (60/3)*60*4;
		auto timenow = std::chrono::system_clock::now();
		auto interval = time_.diff<mtime::hour>(timenow);
		if(interval >= 24){
			rollLog();
		}
	}
	if( length == 0 ){
		//一小时
		if(++flush_everyN_ > 1024){
			output_.flush();
		}
		return;
	}
	//!这是每个文件第一个包到的时候,往里面写时间, 所以可能会比第一行数据慢3秒左右
	//FIXME 
	if( time_init_ ){
		time_init_ = false;
		time_.reset(mtime::year,mtime::second);
		output_.write(time_.getTime(),time_.length());
		output_.write("\n",1);
	}

	int idx=0;
	//* 确认是否写完, 记录下文件传输总数, 以便分档
	int64_t writeBytes = 0;
	do{
		writeBytes += output_.write(data+writeBytes,length-writeBytes);
		++idx;
		if(idx > 3){ LOG_FATAL << "log ofstream write fault"; }
	}while(writeBytes<length-3);

	if(output_.getFileSize() > roll_size_){
		rollLog();
	}
}

void logFile::rollLog(){
	historyOnceInit();

	output_.flush();
	total_size_ += output_.getFileSize();

	mtime tmp(mtime::day,mtime::second);

	//*输出log归档文件名格式
	//!
	std::string rollname = output_.getDirName()+"/"+output_.getStemName()+"_"+time_.getTime()+"<->"+
		tmp.getTime()+output_.getExtension();

	file_history_.emplace_back(
		//path/stem_timeBegin>>timeEnd.extension
		rollname
		,output_.getFileSize());

	fs::rename(file_path_,rollname);

	refreshHistory();

	//重置下一块的时间
	time_init_ = true;
	output_.reset(file_path_,std::ios::app);

	//是否需要清除历史文件
	//*如果文件总大小超过 就删除
	bool is_modify_history_ = false;
	auto it = file_history_.begin();
	while(total_size_ > total_size_max_ && it != file_history_.end()){
		is_modify_history_ = true;
		total_size_ -= it->second;
		if(fs::exists(it->first)){
			fs::remove(it->first);
		}
		it = file_history_.erase(it); // 删除节点
	}
	if(is_modify_history_) refreshHistory();
}

void logFile::historyOnceInit(){
	if(history_once_init_){
		history_once_init_ = false;

		// 打开文件进行读取
		std::ifstream file(history_record_path_);

		std::string line;
		while (std::getline(file, line)) {
			std::istringstream iss(line);
			std::string key;
			int64_t value;

			if (iss >> key >> value) {
				file_history_.emplace_back(key, value);
				total_size_ += value;
			}
		}
		file.close();
	}
}

void logFile::refreshHistory(){
	// 打开文件进行写入
    std::ofstream file(history_record_path_);

    for (const auto& entry : file_history_) {
        file << entry.first << " " << entry.second << "\n";
    }

	file.flush();
    // 关闭文件
    file.close();
}
} // namespace abcd::util