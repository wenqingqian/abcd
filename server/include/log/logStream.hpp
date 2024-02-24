#pragma once

#include "noncopyable.hpp"
// #include "Logger.hpp"	
// #include <concepts>
#include <type_traits>
#include <cstring>
#include <string>

namespace abcd{
using std::string;
constexpr static size_t buffer_size_large = 4000*1000;//4MB
constexpr static size_t buffer_size_small = 4000;//4KB
constexpr static size_t numeric_size 	  = 32;

template <int size>
class logBuffer : public noncopyable{
public:
	logBuffer    ()	{ cur_ = buffer_; 			  }
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


class logStream : public noncopyable{
public:
	using LogLevel = int;
	
	logStream(LogLevel l):level_(l){}

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
	//logStream封装了对buffer的添加
	logBuffer<buffer_size_small> buffer_;

public:
	template <class T, typename = std::enable_if_t<
				std::is_convertible_v<T,int>>>
	logStream& operator << (T val){
		appendInt(static_cast<long long>(val));
		return *this;
	}
	logStream& operator << (float);
	logStream& operator << (double);
	logStream& operator << (long double);
	logStream& operator << (char);
	logStream& operator << (const char*);
	logStream& operator << (string&&);
	logStream& operator << (string&);
	logStream& operator << (bool);
};

}//namespace abcd