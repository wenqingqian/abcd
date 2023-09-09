
#include "LogStream.hpp"
#include "TomlConfig.hpp"
namespace tsuki::util{
const char digits[] = "9876543210123456789";
const char* zero = digits + 9;


void LogStream::appendInt(long long v){
	char* p = buffer_.current();
	long long i = v;
	do {
		int lsd = i % 10;
		i /= 10;
		*p++ = zero[lsd];
	} while (i != 0);
	if (v < 0) {
		*p++ = '-';
	}
	*p = '\0';
	std::reverse(buffer_.current(), p);
	buffer_.addLength(p-buffer_.current());
}

LogStream& LogStream::operator << (float f){
	return operator << (static_cast<double>(f));
}
LogStream& LogStream::operator << (double d){
	if(buffer_.avail() > numeric_size){
		int size = snprintf(buffer_.current(), numeric_size, "%.12g", d);
		buffer_.addLength(size);
	}
	return *this;
} 
LogStream& LogStream::operator << (long double d){
	if(buffer_.avail() > numeric_size){
		int size = snprintf(buffer_.current(), numeric_size, "%.12Lg", d);
		buffer_.addLength(size);
	}
	return *this;
}

LogStream& LogStream::operator << (char c){
	buffer_.append(&c,1);
	return *this;
}
LogStream& LogStream::operator << (const char* c){
	if(c) {
		buffer_.append(c,strlen(c));
	}else{
		buffer_.append("(null)", 6);
	}
	return *this;
}
LogStream& LogStream::operator << (string&& s){
	buffer_.append(s);
	return *this;
}
LogStream& LogStream::operator << (string& s){
	buffer_.append(s);
	return *this;
}
LogStream& LogStream::operator << (bool b){
	buffer_.append((b?"true":"false"),(b?4:5));
	return *this;
}
}//namespace tsuki::util