#ifndef ABCD_UTIL_LOGFILE_HPP
#define ABCD_UTIL_LOGFILE_HPP
#include <string>
#include <fstream>
#include <filesystem>
#include <list>
#include "TomlConfig.hpp"
#include "noncopyable.hpp"
#include "Time.hpp"
#include "FileUtil.hpp"
namespace abcd::util{

namespace fs = std::filesystem;

class LogFile : public noncopyable{
public:
	LogFile();
	~LogFile(){ output_.flush();output_.close(); }
	void flush() { output_.flush(); }
	void append(const char*,int);
	// void resetFlush(int v = 1024) { flush_ = v; }
private:
	//!FIXME 未归档的日志 中途断开会导致当前日志块初始时间清空, 下次打开时会重新设置时间
	//增加一个文件专门用来记录当前块的初始时间, 或者每创建一个新文件将时间放到顶部
	//?第二种方法存在一个问题就是为了获取时间用ifstream就读取第一行时, 性能开销大小跟文件大小有关吗
	//*目前打算用第二种方法, 多增加一个文件有点乱
	void rollLog();
	//加载历史log记录到内存
	void historyOnceInit();
	//删除历史文件后, 刷新历史文件记录
	void refreshHistory();
private:
	//初始化时间, 在每个文件拿到第一个包的时候
	bool time_init_ = true;
	Time time_;
	File output_;
	//单个文件回滚大小
	int64_t roll_size_ = 20*1024*1024;//默认20MB

	//每隔这么久检查下时间, 每天至少保证归档一次
	int time_check_interval_ = (60/3)*60*4;//4hour

	//文件相对路径
	fs::path file_path_;

	int flush_everyN_=0;

	//所有log总大小
	int64_t total_size_ = 0;
	//所有log总大小限制
	int64_t total_size_max_ = 4LL*1024*1024*1024; //4GB

	fs::path history_record_path_;
	bool history_once_init_ = true;
	//历史文件队列 名字跟大小pair
	std::list<std::pair<std::string,int64_t>> file_history_;
};


} // namespace abcd::util

#endif