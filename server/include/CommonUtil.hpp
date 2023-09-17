#ifndef TSUKI_UTIL_COMMONUTIL_HPP
#define TSUKI_UTIL_COMMONUTIL_HPP
#include <string>
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
template <int BEGIN = 1, int END = 64, class T>
inline std::string enumName(T n){
	std::string name;
	staticForEnumName<BEGIN,END>([&]<int BEG>(){
		if( n == static_cast<T>(BEG) ) {	
			[&]<class U, U N>(){
				std::string a = __PRETTY_FUNCTION__;
				auto pos = a.find("U N = ");
				if(pos == std::string::npos) return;
				pos += 6;
				auto pos2= a.find_first_of(";]",pos);
				name = a.substr(pos,pos2-pos);
			}.template operator()<T,static_cast<T>(BEG)>();
		}
	});
	//如果超界了, 就返回enum类名, 如color
	return name.empty()?[]<class U>(){
		std::string a = __PRETTY_FUNCTION__;
		auto pos = a.find("U = ");
		pos += 4;
		auto pos2= a.find_first_of(";]",pos);
		return a.substr(pos,pos2-pos);
	}.template operator()<T>():name;
}
#endif
} // namespace reflect

} // namespace tsuki::util
#endif