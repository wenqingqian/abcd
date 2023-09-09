#ifndef TSUKI_UTIL_LOGSTREAM_HPP
#define TSUKI_UTIL_LOGSTREAM_HPP


#include "noncopyable.hpp"
// #include "Logger.hpp"	
// #include <concepts>
#include <type_traits>
#include <cstring>
#include <string>

namespace tsuki::util{
using std::string;
constexpr static size_t buffer_size_large = 4000*1000;//4MB
constexpr static size_t buffer_size_small = 4000;//4KB
constexpr static size_t numeric_size 	  = 32;

template <int size>
class LogBuffer : public noncopyable{
public:
	LogBuffer    ()	{ cur_ = buffer_; 			  }
	int   avail  () { return size-(cur_-buffer_); }
	int   length () { return cur_-buffer_; 		  }
	void  reset  ()	{ cur_ = buffer_; 			  }
	void  bzero  () { memset(buffer_,0,size); 	  }
	char* current() { return cur_; 				  } 
	const char*	data () { return buffer_; 		  }
	void  addLength(int val) { cur_ += val; }
	void  append(const char* c, int v){
		if(c){
			memcpy(cur_,c,v);
			addLength(v);
		}
	}
	
	void append(const string& s) { append(s.c_str(),s.size()); }
private:
	char* cur_;
	char  buffer_[size];
};

using LogLevel = int;

class LogStream : public noncopyable{
public:
	LogStream(LogLevel l):level_(l){}

	int length() { return buffer_.length(); }
	void reset() { buffer_.reset(); }
	const char* data() { return buffer_.data(); }
	void append(const char* b,int s){
		buffer_.append(b,s);
	}
	void append(const string& s){
		buffer_.append(s);
	}
	void appendInt(long long);

	LogLevel level_;
private:
	//LogStream封装了对buffer的添加
	LogBuffer<buffer_size_small> buffer_;

public:
	template <class T, typename = std::enable_if_t<
				std::is_convertible_v<T,int>>>
	LogStream& operator << (T val){
		appendInt(static_cast<long long>(val));
		return *this;
	}
	LogStream& operator << (float);
	LogStream& operator << (double);
	LogStream& operator << (long double);
	LogStream& operator << (char);
	LogStream& operator << (const char*);
	LogStream& operator << (string&&);
	LogStream& operator << (string&);
	LogStream& operator << (bool);
};

}//namespace tsuki::util

#endif