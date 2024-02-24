#include"TomlConfig.hpp"
#include<iostream>
#include<thread>

//! 未更新, 可能运行不了
int main(){
	// if constexpr (HasNodeMember<cs>) {
	// 	std::cout<<1<<std::endl;
	// }else {
	// 	std::cout<<2<<std::endl;
	// }
	// auto s = cs{};
	// fun(s);

	using namespace std;
	// std::variant<int,std::string> var("1");
	// // std::cout<<get<0>(var);
	// auto* res = std::get_if<0>(&var);
	// if(res){
	// 	std::cout<<*res;
	// }
	using namespace abcd::util;
	// abcd::util::config::init("f.toml");
	auto&& s = abcd::util::config::getInstance();   

	cout<<"test1: basic key-value and escape-character and comment******************************************"<<endl;
	// r="z"
	// [test_1]
	// r = "s\\\n\"[]#d"#a
	// s = 'sd\"s'
	// #inin
	//
	// [test_2]
	// r = "."
	// s = "1.2 3"
	{
		auto p1 = s["r"].value<config::String>();
		cout<<"test1 "<<(p1?*p1:"nullopt")<<endl;
		auto p2 = s["test_1"]["r"].value<config::String>();
		cout<<"test1 "<<(p2?*p2:"nullopt")<<endl;
		auto p3 = s["test_1"]["s"].value<config::String>();
		cout<<"test1 "<<(p3?*p3:"nullopt")<<endl;
		auto p4 = s["test_2"]["r"].value<config::String>();
		cout<<"test1 "<<(p4?*p4:"nullopt")<<endl;
		auto p5 = s["test_2"]["s"].value<config::String>();
		cout<<"test1 "<<(p5?*p5:"nullopt")<<endl;
	}

	cout<<"test2: session vector and int******************************************"<<endl;
	// [[test_3]]
	// r = 1
	// f = "sd"
	// [[test_3]]
	// r =10
	// s= 1
	{
		auto p1 = s["test_3"][0]["r"].value<config::Int>();
		cout<<"test2 "<<(p1?*p1:-1)<<endl;
		auto p2_= s["test_3"][0]["f"].value<config::String>();
		cout<<"test2 "<<(p2_?*p2_:"nullopt")<<endl;

		auto p3 = s["test_3"][1]["r"].value<config::Int>();
		cout<<"test2 "<<(p3?*p3:-1)<<endl;
		auto p4 = s["test_3"][1]["s"].value<config::Int>();
		cout<<"test2 "<<(p4?*p4:-1)<<endl;
	}

	cout<<"test3: vector value******************************************"<<endl;
	// f = [1,"a"]
	// [[test_4]]
	// r = [1 ,   4]
	// s =[ 1,2, "[],,1\\\"\n#1",'\n'] 
	{

		auto p1 = s["test_4"][0]["r"][0].value<config::Int>();
		cout<<"test3:1 "<<(p1?*p1:-1)<<endl;
		auto p2 = s["test_4"][0]["r"][1].value<config::Int>();
		cout<<"test3:2 "<<(p2?*p2:-1)<<endl;

		auto p3 = s["f"][0].value<config::Int>();
		cout<<"test3:3 "<<(p3?*p3:-1)<<endl;
		auto p3_= s["f"][1].value<config::String>();
		cout<<"test3:4 "<<(p3_?*p3_:"nullopt")<<endl;

		auto p4 = s["test_4"][0]["s"][0].value<config::Int>();
		cout<<"test3:5 "<<(p4?*p4:-1)<<endl;
		auto p5 = s["test_4"][0]["s"][1].value<config::Int>();
		cout<<"test3:6 "<<(p5?*p5:-1)<<endl;
		auto p6 = s["test_4"][0]["s"][2].value<config::String>();
		cout<<"test3:7 "<<(p6?*p6:"nullopt")<<endl;
		auto p7 = s["test_4"][0]["s"][3].value<config::String>();
		cout<<"test3:8 "<<(p7?*p7:"nullopt")<<endl;

	}

	cout<<"test4: muti key******************************************"<<endl;
	// a. a .c=1
	// a .b= "s\\\n\"[]#d"#a"
	// a.c = [1,"2"]
	//
	// [[test_5]]
	//
	// a.c = 3
	// a.d = [1,"2","3"]
	//
	// [[test_5]]
	// a. e .a=1
	{
		auto p6 = s["a"]["c"][0].value<config::Int>();
		cout<<"test4:0 "<<(p6?*p6:-1)<<endl;
		auto p7 = s["a"]["c"][1].value<config::String>();
		cout<<"test4:0 "<<(p7?*p7:"nullopt")<<endl;

		if(auto p1 = s["a"]["a"]["c"].value<config::Int>()) cout<<"test4:1 "<<*p1<<endl;
		
		if(auto p2 = s["a"]["b"].value<config::String>()) cout<<"test4:2 "<<*p2<<endl;

		if(auto p3 = s["test_5"][0]["a"]["c"].value<config::Int>()) cout<<"test4:3 "<<*p3<<endl;

		if(auto p3_= s["test_5"][0]["a"]["d"][0].value<config::Int>()) cout<<"test4:4 "<<*p3_<<endl;
		
		if(auto p4 = s["test_5"][0]["a"]["d"][1].value<config::String>()) cout<<"test4:5 "<<*p4<<endl;

		if(auto p5 = s["test_5"][0]["a"]["d"][2].value<config::String>()) cout<<"test4:6 "<<*p5<<endl;

		if(auto p8= s["test_5"][1]["a"]["e"]["a"].value<config::Int>()) cout<<"test4:7 "<<*p8<<endl;
		
	}

	cout<<"test5: bool 有BUG但不多.*****************************************************"<<endl;
	// [test_6]
	//
	// a = true
	// b.a = false
	// c = "true"
	// b.b = false
	// b.c = [true,"true",false,"false"]
	// b.d = [1,true,]
	{
		if(auto p1 = s["test_6"]["a"].value<config::Bool>()) cout<<"test5:1 "<<*p1<<endl;
		if(auto p2 = s["test_6"]["b"]["a"].value<config::Bool>()) cout<<"test5:2 "<<*p2<<endl;
		if(auto p3 = s["test_6"]["c"].value<config::String>()) cout<<"test5:3 "<<*p3<<endl;
		if(auto p4 = s["test_6"]["b"]["b"].value<config::Bool>()) cout<<"test5:4 "<<*p4<<endl;
		if(auto p5 = s["test_6"]["b"]["c"][0].value<config::Bool>()) cout<<"test5:5 "<<*p5<<endl;
		if(auto p6 = s["test_6"]["b"]["c"][1].value<config::String>()) cout<<"test5:6 "<<*p6<<endl;
		if(auto p7 = s["test_6"]["b"]["c"][2].value<config::Bool>()) cout<<"test5:7 "<<*p7<<endl;
		if(auto p8 = s["test_6"]["b"]["c"][3].value<config::String>()) cout<<"test5:8 "<<*p8<<endl;
		if(auto p9 = s["test_6"]["b"]["d"][0].value<config::Int>()) cout<<"test5:9 "<<*p9<<endl;
		if(auto p0 = s["test_6"]["b"]["d"][1].value<config::Bool>()) cout<<"test5:10 "<<*p0<<endl;
	}

	/*
	*	修改数据可以直接像这样拿到原始variant修改(使用&)
	*	目前怀疑不需要做线程安全
	*
	*	可以修改节点类型
	*	如果修改了节点类型, 另外使用的地方处理TODO
	*/
	// auto& p9 = s["test_6"]["b"]["d"][0].node;
	// p9 = 2;
	if(auto p9 = s["test_6"]["b"]["d"][0].value<config::Int>()) cout<<"test5:9 "<<*p9<<endl;

	s["test_6"]["b"]["d"][0].write(3);
	if(auto p9 = s["test_6"]["b"]["d"][0].value<config::Int>()) cout<<"test5:9 "<<*p9<<endl;

	
}

