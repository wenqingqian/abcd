// #include "include/CommonUtil.hpp"
#include <iostream>
using namespace std;
// using namespace tsuki::util;

#if defined(NDEBUG)
#define DEBUGP(x) \
	[](){}()
#else
#define DEBUGP(x) \
	[&](){std::cout<<x<<std::endl;}()
#endif

int main(){
	int i= 9;
	DEBUGP(i<<2<<"sd");
}