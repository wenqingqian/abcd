// #include "include/CommonUtil.hpp"
#include <iostream>
#include <map>
#include <type_traits>
#include <utility>
using namespace std;
// using namespace abcd::util;

#if defined(NDEBUG)
#define DEBUGP(x) \
	[](){}()
#else
#define DEBUGP(x) \
	[&](){std::cout<<x<<std::endl;}()
#endif

struct t{
	int i;
};

template<class Tp>
struct is_pair{
	template <class Up, class = decltype(Up::first)> 
	static constexpr has_member_first(){}
	

};
#include <memory>
int main(){
	// DEBUGP(i<<2<<"sd");
}