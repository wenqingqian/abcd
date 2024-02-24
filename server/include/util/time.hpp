#pragma once
/*
 * 大部分代码用于异步日志构建固定格式的时间字符串
 * 比如在log文件顶部记录一个时间字符串, 在重启服务器时可以读取这个字符串得到上次log的日期
*/
#include "debug.hpp"
#include <chrono>
#include <cstdio>
#include <string>
#include <sstream>
#include <type_traits>
namespace abcd
{
class mtime {
public:
	enum mode{
		year,
		month,
		day,
		hour,
		minute,
		second,
		millisecond,
		microsecond
	};
public:
	mtime();
	//*构造时间字符串
	mtime(mode from, mode end);

	void reset();
	//*相当于构造函数
	void reset(mode from, mode end);
	//*按2000-01-01~12:00:00固定格式解析, 不支持毫秒微秒
	//*目前专门用于如果log重启了, 从最近的log的顶部拿取log初始时间
	void reset(const std::string& str);
	//* 从一个time_point构造, 默认按year-microsecond全写入getTime
	void reset(std::chrono::system_clock::time_point &tp);
	
	void resetbuf(){ cur_ = timebuf_; }
	int length(){ return cur_-timebuf_; }
	const char* getTime(){ return timebuf_; }

private:
	template <mtime::mode m_> struct checktime      		
	{using type = std::chrono::seconds;};
	template <mtime::mode m_>
	using checktime_t = typename checktime<m_>::type;
public:
	
	//*计算两个time之间的距离, 按给定mode返回
	template <mode T, typename U = checktime_t<T>> 
	int diff(std::chrono::system_clock::time_point &);
	template <mode T, typename U = checktime_t<T>> 
	int diff(mtime &);

private:
	int time_epoch(auto m);
	const char* modeFormat(auto m);
	//前置分隔符
	const char* forwardDelimiter(auto m);
	const char* forwardFormat(auto m);

	int avail(){ return 64-(cur_-timebuf_); }

	void addl(int l){ cur_ += l; }

	std::chrono::system_clock::time_point& tp(){ return time_point_; }

private:
	char *cur_;
	char timebuf_[64];
	std::chrono::system_clock::time_point time_point_;
	tm tm_;
};

template <> struct mtime::checktime<mtime::hour>		
{using type = std::chrono::hours;};
template <> struct mtime::checktime<mtime::minute>		
{using type = std::chrono::minutes;};
template <> struct mtime::checktime<mtime::second>		
{using type = std::chrono::seconds;};
template <> struct mtime::checktime<mtime::millisecond>
{using type = std::chrono::milliseconds;};
template <> struct mtime::checktime<mtime::microsecond>
{using type = std::chrono::microseconds;};


template <mtime::mode T, typename U> 
inline int mtime::diff(std::chrono::system_clock::time_point &tp){
	return std::chrono::duration_cast<U>(time_point_ - tp).count();
}

template <mtime::mode T, typename U> 
inline int mtime::diff(mtime &t){
	return std::chrono::duration_cast<U>(time_point_ - t.tp()).count();
}

} // namespace abcd