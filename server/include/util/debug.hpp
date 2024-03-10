#pragma once
#include <iostream>
#include <cstring>
namespace abcd{
	#if defined(NDEBUG)
	#define output(x) \
		[](){}();
	#else
	#define output(x) \
		[&](){std::cout<<cut(__FILE__)<<" "<<__LINE__<<":"<< x <<std::endl;}();
	#endif

	#define panic(x) \
		[&](){std::cout<<cut(__FILE__)<<" "<<__LINE__<<":"<< x <<std::endl;}(); \
		exit(-1); \

	inline static const char* cut(const char * filePath){
		const char* fileName = strrchr(filePath, '/'); // Find the last occurrence of '/'
		return (fileName != nullptr) ? (fileName + 1) : filePath; // If '/' is found, return the substring after it; otherwise, return the entire string
	}
}
