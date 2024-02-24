#include "time.hpp"
namespace abcd{

mtime::mtime()
	:	cur_(timebuf_),
		time_point_(std::chrono::system_clock::now()),
		tm_()
	{
	auto as_time_t = std::chrono::system_clock::to_time_t(time_point_);
	localtime_r(&as_time_t, &tm_);
}

mtime::mtime(mode from, mode end)
	:	cur_(timebuf_),
		time_point_(std::chrono::system_clock::now()),
		tm_()
	{
	auto as_time_t = std::chrono::system_clock::to_time_t(time_point_);
	localtime_r(&as_time_t, &tm_);
	int timesize = snprintf(cur_, avail(), modeFormat(from), time_epoch(from));
	addl(timesize);
	for(int i=from+1;i<=end;i++){
		timesize = snprintf(cur_,avail(),forwardFormat(i),time_epoch(i));
		addl(timesize);
	}
}

void mtime::reset(){
	resetbuf();
	time_point_ = std::chrono::system_clock::now();
	
	auto as_time_t = std::chrono::system_clock::to_time_t(time_point_);
	localtime_r(&as_time_t, &tm_);
}

void mtime::reset(mode from, mode end){
	reset();
	
	int timesize = snprintf(cur_, avail(), modeFormat(from), time_epoch(from));
	addl(timesize);
	for(int i=from+1;i<=end;i++){
		timesize = snprintf(cur_,avail(),forwardFormat(i),time_epoch(i));
		addl(timesize);
	}
}	

//*按2000-01-01~12:00:00固定格式解析, 不支持毫秒微秒
//*目前专门用于如果log重启了, 从最近的log的顶部拿取log初始时间
void mtime::reset(const std::string& str){
	resetbuf();
	std::istringstream iss(str);
	std::string datePart, timePart;
	std::getline(iss, datePart, '~');
	std::getline(iss, timePart);

	// 解析日期部分
	int year, month, day;
	std::istringstream dateStream(datePart);
	dateStream >> year;
	dateStream.ignore(1); // 忽略 -
	dateStream >> month;
	dateStream.ignore(1); // 忽略 -
	dateStream >> day;

	// 解析时间部分
	int hour, minute, second;
	std::istringstream timeStream(timePart);
	timeStream >> hour;
	timeStream.ignore(1); // 忽略 :
	timeStream >> minute;
	timeStream.ignore(1); // 忽略 :
	timeStream >> second;

	// 构造时间点
	tm_.tm_year = year - 1900;
	tm_.tm_mon = month - 1;
	tm_.tm_mday = day;
	tm_.tm_hour = hour;
	tm_.tm_min = minute;
	tm_.tm_sec = second;

	// 构造time_point
	time_point_ = std::chrono::system_clock::from_time_t(std::mktime(&tm_));

	cur_ = timebuf_;
	int len = snprintf(cur_,sizeof(timebuf_),"%s",str.c_str());
	addl(len);
}

//* 从一个time_point构造, 默认按year-microsecond全写入getTime
void mtime::reset(std::chrono::system_clock::time_point &tp){
	resetbuf();
	time_point_ = tp;
	auto as_time_t = std::chrono::system_clock::to_time_t(time_point_);
	localtime_r(&as_time_t, &tm_);
	
	auto from = year; auto end = microsecond;
	int timesize = snprintf(cur_, avail(), modeFormat(from), time_epoch(from));
	addl(timesize);
	for(int i=from+1;i<=end;i++){
		timesize = snprintf(cur_,avail(),forwardFormat(i),time_epoch(i));
		addl(timesize);
	}
}

int mtime::time_epoch(auto m){
	namespace c = std::chrono;
	switch (m){
		case year		: 	return tm_.tm_year + 1900;break;
		case month		:	return tm_.tm_mon  + 1 	 ;break;
		case day		:	return tm_.tm_mday 		 ;break;
		case hour		:	return tm_.tm_hour 		 ;break;
		case minute		:	return tm_.tm_min 		 ;break;
		case second		:	return tm_.tm_sec 		 ;break;
		case millisecond:	return c::duration_cast<c::milliseconds>
							(time_point_.time_since_epoch()).count() % 1000;
		case microsecond:	return c::duration_cast<c::microseconds>
							(time_point_.time_since_epoch()).count() % 1000;
	
	}
	return 0;
}
const char* mtime::modeFormat(auto m){
	switch (m){
		case year		: 	return "%04d" ;break;
		case month		:	return "%02d" ;break;
		case day		: 	return "%02d" ;break;
		case hour		:	return "%02d" ;break;
		case minute		:	return "%02d" ;break;
		case second		:	return "%02d" ;break;
		case millisecond: 	return "%03d" ;break;
		case microsecond: 	return "%03d" ;break;
	}
	return "";
}
//前置分隔符
const char* mtime::forwardDelimiter(auto m){
	switch (m){
		case year		: 	return ""  ;break;
		case month		:	return "-" ;break;
		case day		: 	return "-" ;break;
		case hour		:	return "~" ;break;
		case minute		:	return ":" ;break;
		case second		:	return ":" ;break;
		case millisecond: 	return " " ;break;
		case microsecond: 	return ":" ;break;
	}
	return "";
}
const char* mtime::forwardFormat(auto m){
	switch (m){
		case year		: 	return "%04d"  	;break;
		case month		:	return "-%02d" 	;break;
		case day		: 	return "-%02d" 	;break;
		case hour		:	return "~%02d" 	;break;
		case minute		:	return ":%02d" 	;break;
		case second		:	return ":%02d" 	;break;
		case millisecond: 	return " %03ld" ;break;
		case microsecond: 	return ":%03ld" ;break;
	}
	return "";
}

}