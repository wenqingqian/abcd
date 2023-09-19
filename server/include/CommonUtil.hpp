/**
 * @author wenqingqian
 * 
 * @compiler:
 * 	gcc (or clang)?
*/

#ifndef TSUKI_UTIL_COMMONUTIL_HPP
#define TSUKI_UTIL_COMMONUTIL_HPP
#include <string>
#include <iostream>
namespace tsuki::util
{
namespace reflect
{
#if __cplusplus > 201402L
template <int BEGIN, int END>
inline void staticForEnumName(auto&& func){
	if constexpr ( BEGIN == END ){
		return;
	}else{
		//https://stackoverflow.com/questions/49392738/\
			is-it-legal-to-explicitly-specify-a-generic\
			-lambdas-operator-template-argumen

		//必须拿到lambda里的operator方法来传模板参数	
		func.template operator()<BEGIN>();
		staticForEnumName<BEGIN+1,END>(func);
	}
}
#endif
#if __cplusplus > 201703L
//非侵入式反射获取enum名字 必须传入原生enum类型数据, 不能转为int等, 不然应该会返回数字本身
//enum 默认从0 开始 BEGIN END 前开后闭
template <int BEGIN = 0, int END = 64, class T>
inline std::string enumToName(T n){
	std::string name;
	staticForEnumName<BEGIN,END>([&]<int BEG>(){
		if( n == static_cast<T>(BEG) ) {	
			[&]<class U, U N>(){
				std::string a = __PRETTY_FUNCTION__;
				auto pos = a.find("U N = ");
				if(pos == std::string::npos) return;
				auto pos2= a.find_first_of(";]",pos);
				auto pos3= a.rfind("::",pos2);
				name = a.substr(pos3+2,pos2-pos3-2);			
			}.template operator()<T,static_cast<T>(BEG)>();
		}
	});
	//如果超界了, 就返回enum类名, 如color
	return name.empty()?[]<class U>(){
		std::string a = __PRETTY_FUNCTION__;
		auto pos = a.find("U = ");
		pos += 4;
		auto pos2= a.find_first_of(";]",pos);
		auto pos3= a.rfind("::",pos2);
		return a.substr(pos3+2,pos2-pos3-2);
	}.template operator()<T>():name;
}

template <class T,int BEGIN = 0, int END = 64>
inline T nameToEnum(const std::string& n){
	if constexpr(BEGIN == END){
		std::string str = "BEGIN == END or dont find enum name :[" + n + "] in " + __PRETTY_FUNCTION__;
		throw std::runtime_error(str.c_str());
	}else{
		if(n == enumToName<BEGIN,BEGIN+1>(static_cast<T>(BEGIN))) {
			return static_cast<T>(BEGIN);
		}else{
			return nameToEnum<T,BEGIN+1,END>(n);
		}
	}
}


} // namespace reflect
#endif

} // namespace tsuki::util
#endif