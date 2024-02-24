/**
*
*	toml parse header-only
*	
*	@support:
*	#comment
*	a = 1 | '1' | true | [1,'1',true]
*	[session]
*	[[sessions]]
*	a.a.a = 1_000_000
*	raw_string = '\n\r\t'
*   
*	@return
*   1. auto& (config_&)      s = config::getInstance();
*	   auto  (std::optional) p = s["sessions"][0]["a"]["a"]["a"].value<config::Int>();
*	2. auto  (std::optional) p = CONFIG["sessions"][0]["a"]["a"]["a"].value<config::Int>();
*
*	@history:
*   1. basic key-value and escape-character and comment
*   2. session vector and int
*   3. vector
*   4. multi key
*   5. bool
*   6. cant get config_.node
*   7. #define CONFIG = getInstance()
*	8. auto init when first get
*   9. support delete
*
*	@todo
*	int64 support
*	float support
*   0x 0b support 
*/


#pragma once

namespace abcd{

// define TomlConfigLog for print log information*/
	#ifndef TomlConfigLog
	#define TomlConfigLog(x) [](){}();
	// #define TomlConfigLog(x) [&](){std::cout<< x << std::endl;}; //<-
	//* it need to overload operator <<
	#endif
}
#include<variant>
#include<cstring>
#include<iostream>
#include<filesystem>
#include<map>
#include<mutex>
#include<fstream>
#include<vector>
#include<functional>
#include<memory>
#include<type_traits>
#include<concepts>
#include<optional>
#include<any>
#include<typeinfo>
#include<assert.h>

namespace abcd{

class noncopyable_toml_config{
	public:
	noncopyable_toml_config(const noncopyable_toml_config&) = delete;
	const noncopyable_toml_config& operator = (const noncopyable_toml_config&) = delete; 
	protected:
	noncopyable_toml_config(){}
	~noncopyable_toml_config(){}
};

namespace fs = std::filesystem;

template <class Tp, class Up>
concept same_type = std::same_as<std::remove_cvref_t<Tp>,std::remove_cvref_t<Up>>;

class configToml : public noncopyable_toml_config{
private:
	configToml(){}
	~configToml(){root.reset();}
private:
template <class t>			using vector  = std::vector<t>;
							using string  = std::string;
template <class...t>		using variant = std::variant<t...>;
template <class a,class b>  using map     = std::map<a,b>;


public:
	struct config_;
private:
	template <class...Args>
	static auto   msc(Args&& ...args){ return std::make_shared<config_>(std::forward<Args>(args)...); }
	static auto&& getInt   (auto&& config){ return std::get<cvtg(Config_Type::Int   )>(config->node); }
	static auto&& getString(auto&& config){ return std::get<cvtg(Config_Type::String)>(config->node); }
	static auto&& getVector(auto&& config){ return std::get<cvtg(Config_Type::Vector)>(config->node); }
	static auto&& getMap   (auto&& config){ return std::get<cvtg(Config_Type::Map   )>(config->node); }
	template <class T>
	static T* getIf(auto&& config){ return std::get_if<T>(&config->node); }

	using config_sp = std::shared_ptr<config_>;

public:
	//* config type support
	using Int 		= int32_t;
	using String 	= string;
	using Bool      = bool;
	using Float     = _Float32;//TODO
	using Long		= int64_t; //TODO
	using Double    = _Float64;//TODO
	using Vector 	= vector<config_sp>;
	using Map		= map<string,config_sp>;

public:
	//*节点
	struct config_ {
		friend class configToml;
		//* 暂时设为不允许直接拿到, 必须用内置方法
		//* 因此额外添加构造函数
		config_(){}
		template<class T>
		config_(T val){ node = val; }

		private:
		variant<Int,String,Bool,Float,Long,Double,Vector,Map> node;
		public:
		//* 修改数据类型
		//no use
		template <class T>	//obsolete
		void write(T&& val){
			node = val;
		}
		
		//* 尝试提取的数据类型	
		template <class T>
		std::optional<T> value(char* copyout = nullptr){

			if ( this == &default_ ) return std::nullopt;

			// *在__attribute__((constructor))中无法使用string
			// *将字符串信息拷贝到给定地址
			if(copyout){
				if constexpr( same_type<T,string> ){
					const char* str = std::get<String>(node).c_str();
					memmove(copyout, str, strlen(str)+1);
					return "";
				}
				assert(0);
			}
			
			// * 暂时分开算, 或者统一处理特判bool
			if constexpr ( same_type<T,Bool> ){
				//!
				//	BUG: 文件里是true这里会返回false, 文件里是false会返回true
				//  在解析完往config_里塞msc(Bool())时解析没问题, 塞进Bool(true)后这里会得到false
				//	所以这里单独取个反
				if(auto ptr = std::get_if<Bool>(&node)){
					return !*ptr;
				}else{
					TomlConfigLog("attempt to get element as bool while it's not");
					return std::nullopt;
				}
				return std::nullopt;
			}else{
				if (std::holds_alternative<T>(node)) {
					return std::get<T>(node);
				}
			}
			return std::nullopt;
		}
		template <class T>
		config_& operator[](T&& key) {
			//*字符串或数字
			if constexpr ( same_type<T,Int> ){
				if(auto ptr = std::get_if<Vector>(&node)){
					if(key < 0 || key >= static_cast<int>(ptr->size())){
						TomlConfigLog("attempt to index out of bound");
						return default_;
					}
					return *((*ptr)[key]);
				}else{
					TomlConfigLog("key " << key << " is not a vector");
					return default_;
				}
				return default_;
			}else{
				if(auto ptr = std::get_if<Map>(&node)){
					auto it = ptr->find(key);
					if(it != ptr->end()){
						return *(it->second);
					}else{
						TomlConfigLog("cant find key " << key << " in map");
						return default_;
					}
					return default_;
				}else{
					TomlConfigLog("key " << key << " is not a map or check your [key] type");
					return default_;
				}
			}
			TomlConfigLog("config[] error");
			return default_;
		}
	};
private:
	//*默认错误返回占位, value获取为nullopt
	static config_ default_;

	//*根表, 是一张map
	config_sp root;

	//*默认根session
	string base_session_ = " ";
	//*配置文件路径
	string file_path_;

private:
	enum class Config_Type: uint8_t{
		//* basic type
		Int = 0,
		String,
		Bool,
		Float,
		Long,
		Double,
		Vector,
		Map,
		//* special type
		Null,
		Escape
	};

	//for std::get<con>(var)
	//	useless but some historical code rely on 
	constexpr static uint64_t cvtg(Config_Type ct){
		return static_cast<uint64_t>(ct);
	}
	//* 解析状态
	enum class Parse:int{
		Value=0,	//常规值
		Comment,	//注释
		Session,	//
		Default,	//
		Vector,		//
		Vector_Session,//
		End,		//不需要后处理
	};

	//* 解析状态机内状态
	struct parse_info: public noncopyable_toml_config{
		// parse_info(string b, Parse s):buffer(b),state(s){}
		string buffer;	//缓冲区
		Parse state;	//解析状态保存, 用于后处理
		Config_Type ct; //节点类型
		string key;		//键
		string value;	//基本值
		int value_i;	//int值
		vector<std::any> vany;	//数组
		int vanyidx;	//数组大小, any缓冲区, 不清空
		int block; 		//数组解析区块数

		vector<string> sessionv;  //多级表键缓冲
		int sevidx;  //大小

		//是不是多级表
		bool is_mutlisession;
		//* 不实现为单例, 不然还要主动释放
	};
private:
	//*获取文件路径
	string getFilepath() { return file_path_; }
	//*解析文本数据
	void parse(const string&);
public:

	void setFilepath(const char* str){
		file_path_ = str;
	}
	void release(){
		this->~configToml();
	}
	//*单例类
	static configToml& getInstance(){
		static configToml instance;
		return instance;
	}
	//*第一层索引
	//try delete const config*this to use once_init
	config_& operator[](const string& key){
		once_init();
		return *(getMap(root)[key]);
	}
private:
	std::mutex mu;
	int once_init_ = 0;
	//*初始化once
	void once_init(){
		if(once_init_ == 0){
			std::lock_guard<std::mutex> lg(mu);
			if(once_init_ == 0){
				once_init_++;
				init();
			}
		}
	}
	//*初始化
	void init(){
		root = msc(Map());
		getMap(root)[" "] = msc(Map());
		parse(file_path_);
	}
};




//!
//*解析文件
inline void configToml::parse(const string& path){
	fs::path filePath = path;
	if (fs::exists(filePath)) {
		std::ifstream inputFile(filePath);
		if (inputFile.is_open()) {
		
			//********************************************************begin parse
			parse_info pinfo;
			auto parseLine = [&](string& line, auto parse_state) {
				int line_size = line.size();
				bool valueing = false;
				for(int i = 0;i<line_size;++i){
					char c = line[i];
					switch (c){
						case ' ':
							switch (parse_state){
								case Parse::Vector:
									if(pinfo.ct != Config_Type::Escape) break;
									[[fallthrough]];
								case Parse::Value:
									//* if is value, dont pass
									pinfo.value += c;
									break;
								default:
									break;
							}
							break;
						case '#':
							switch (parse_state){
								case Parse::Default:
									pinfo.state = Parse::Comment;
									return;
									break;
								case Parse::Vector:
									if( pinfo.ct != Config_Type::Escape) break;
									[[fallthrough]];
								case Parse::Value:
									if(valueing) pinfo.value += c;
									break;
								default:
									break;
							}
							break;
						case '[':
							switch (parse_state){
								case Parse::Vector:
									if( pinfo.ct != Config_Type::Escape) break;
									[[fallthrough]];
								case Parse::Value:
									//* if dont encounter ' " after = , it's vector
									if(pinfo.ct == Config_Type::Null){
										pinfo.vanyidx = 0;
										pinfo.ct = Config_Type::Vector;
										pinfo.state = Parse::Vector;
										parse_state = Parse::Vector;
										pinfo.block = 0;
										break;
									}
									pinfo.value += c;
									break;
								case Parse::Default:
									parse_state = Parse::Session;
									// pinfo.state = Parse::Session;
									break;
								case Parse::Session:
									parse_state = Parse::Vector_Session;
									// pinfo.state = Parse::Vector_Session;
									break;
								default:
									break;
							}
							break;
						case ']':
							switch (parse_state){
								case Parse::Vector_Session:
									[[fallthrough]];
								case Parse::Session:
									pinfo.state = parse_state;
									return;
									break;
								case Parse::Vector:
									if( pinfo.ct != Config_Type::Escape ){
										if( pinfo.ct == Config_Type::Bool ){
											pinfo.ct == Config_Type::Vector;
											if(pinfo.value.find("true")){
												if( pinfo.vanyidx>=pinfo.vany.size() ){
													pinfo.vany.emplace_back(true);
													pinfo.vanyidx++;	
												}else{
													pinfo.vany[pinfo.vanyidx++] = true;
												}
											}else if(pinfo.value.find("false")){
												if( pinfo.vanyidx>=pinfo.vany.size() ){
													pinfo.vany.emplace_back(false);
													pinfo.vanyidx++;	
												}else{
													pinfo.vany[pinfo.vanyidx++] = false;
												}
											}
											pinfo.value = "";
										}
										break;
									}
									[[fallthrough]];
								case Parse::Value:
									pinfo.value += c;
									break;
								default:
									break;
							}
							break;
						case '.':
							switch (parse_state){
								case Parse::Default:
									//* 多级表
									pinfo.is_mutlisession = true;
									if( pinfo.sevidx>=pinfo.sessionv.size() ){
										pinfo.sessionv.emplace_back(pinfo.buffer);
										pinfo.buffer = "";
										pinfo.sevidx++;	
									}else{
										pinfo.sessionv[pinfo.sevidx++] = pinfo.buffer;
										pinfo.buffer = "";
									}
									break;
								case Parse::Vector:
									if( pinfo.ct != Config_Type::Escape) break;
									[[fallthrough]];	
								case Parse::Value:
									pinfo.value += c;
									break;
								default:
									break;
							}
							break;
						case '=':
							switch (parse_state){
								case Parse::Default:
									if(pinfo.is_mutlisession){
										if( pinfo.sevidx>=pinfo.sessionv.size() ){
											pinfo.sessionv.emplace_back(pinfo.buffer);
											pinfo.sevidx++;	
										}else{
											pinfo.sessionv[pinfo.sevidx++] = pinfo.buffer;
										}
									}else{
										pinfo.key = pinfo.buffer;
									}
									pinfo.value="";
									pinfo.state = Parse::Value;
									parse_state = Parse::Value;
									while(i+1<line_size){
										if(line[i+1] == ' ') ++i;
										else break;
									}
									break;
								case Parse::Vector:
									if( pinfo.ct != Config_Type::Escape) break;
									[[fallthrough]];
								case Parse::Value:
									pinfo.value += c;
									break;
								default:
									parse_state = Parse::Value;
									break;
							}
							break;
						case '\"':
							switch (parse_state){
								case Parse::Default:
									break;
								case Parse::Value:
									pinfo.ct = Config_Type::Escape;
									valueing = !valueing;
									if(!valueing) return;
									break;
								case Parse::Vector:
									pinfo.ct = Config_Type::Escape;
									valueing = !valueing;
									//* 读到尾", 通过判断CT , 以 value, string 方式获取这一个值
									if(!valueing){

										if( pinfo.vanyidx>=pinfo.vany.size() ){
											pinfo.vany.emplace_back(pinfo.value);
											pinfo.vanyidx++;	
										}else{
											pinfo.vany[pinfo.vanyidx++] = pinfo.value;
										}
										pinfo.value = "";
										pinfo.ct = Config_Type::Vector;
									}
									break;
								default:
									break;
							}
							break;
						case '\'':
							switch (parse_state){
								case Parse::Default:
									break;
								case Parse::Value:
									pinfo.ct = Config_Type::String;
									break;
								case Parse::Vector:{
									//* 直接读到下一个 '
									auto pos = line.find('\'',i+1);
									if( pinfo.vanyidx>=pinfo.vany.size() ){
										pinfo.vany.emplace_back(line.substr(i+1,pos-i-1));
										pinfo.vanyidx++;	
									}else{
										pinfo.vany[pinfo.vanyidx++] = line.substr(i+1,pos-i-1);
									}
									i = pos+1;
									break;
								}
								default:
									break;
							}
							break;
						case '\\':
							if(pinfo.ct == Config_Type::Escape){
								switch (parse_state){
									case Parse::Vector:
										if(pinfo.ct != Config_Type::Escape) break;
										[[fallthrough]];
									case Parse::Value:
										if(i+1<line_size){
											char cc = line[i+1];
											//* 转义
											switch (cc){												
												case 't': pinfo.value += '\t'; break;
												case 'r': pinfo.value += '\r'; break;
												case 'n': pinfo.value += '\n'; break;
												case '\\': pinfo.value += '\\'; break;
												case '"': pinfo.value += '"'; break;
												default: pinfo.value += cc; break;
											}
										}else{
											TomlConfigLog("use single \\ at the end of escape string in toml");
										}
										i++;
										break;
									
									default:
										break;
								}
								break;
							}else if(pinfo.ct == Config_Type::String){
								switch (parse_state){
									case Parse::Value:
										pinfo.value += c;
										pinfo.value += line[i+1];
										i++;
										break;
									
									default:
										break;
								}
								break;
							}
						default:
							switch (parse_state){
								case Parse::Vector_Session:
									[[fallthrough]];
								case Parse::Session:
									pinfo.buffer += c;
									break;
								case Parse::Default:
									pinfo.buffer += c;
									break;
								case Parse::Vector:
									[[fallthrough]];
								case Parse::Value:
									//* 数组或者数字
									if(((parse_state == Parse::Vector && pinfo.ct != Config_Type::Escape)
										|| pinfo.ct == Config_Type::Null)
										&& c>='0' && c<='9')
									{
										pinfo.value_i = c-'0';
										i++;
										//* 计算数字
										while(i<line_size && ((line[i]>='0' && line[i]<='9') || line[i]=='_')){
											if(line[i]!='_') pinfo.value_i = pinfo.value_i*10 + (line[i]-'0');
											i++;
										}
										if(parse_state == Parse::Vector){
											if( pinfo.vanyidx>=pinfo.vany.size() ){
												pinfo.vany.emplace_back(pinfo.value_i);
												pinfo.vanyidx++;	
											}else{
												pinfo.vany[pinfo.vanyidx++] = pinfo.value_i;
											}
											break;
										}
										pinfo.ct = Config_Type::Int;
										break;
									}
									//* ,
									if( c == ',' && (pinfo.ct == Config_Type::Vector 
										|| (parse_state == Parse::Vector && pinfo.ct == Config_Type::Bool))){
										if( pinfo.ct == Config_Type::Bool ){
											pinfo.ct == Config_Type::Vector;
											if(pinfo.value.find("true")){
												if( pinfo.vanyidx>=pinfo.vany.size() ){
													pinfo.vany.emplace_back(true);
													pinfo.vanyidx++;	
												}else{
													pinfo.vany[pinfo.vanyidx++] = true;
												}
											}else if(pinfo.value.find("false")){
												if( pinfo.vanyidx>=pinfo.vany.size() ){
													pinfo.vany.emplace_back(false);
													pinfo.vanyidx++;	
												}else{
													pinfo.vany[pinfo.vanyidx++] = false;
												}
											}
											pinfo.value = "";
										}
										break;
									}
									//* vector bool
									if(parse_state == Parse::Vector && pinfo.ct == Config_Type::Vector){
										//! 单值跟数组存bool逻辑不同, 单值直接value存好, 改ct, string就按string处理, bool就查找true还是false
										//! 数组bool 解析时直接判断存进vec<any> 如上
										pinfo.ct = Config_Type::Bool;
									}


									//* bool
									if( pinfo.ct == Config_Type::Null) {
										pinfo.ct = Config_Type::Bool;
									}

									pinfo.value += c;
									break;
								default:
									break;
							}
							break;
					}
				}
				// pinfo.state = Parse::Default;
				return;
			};

			// auto state = Config_Type_State::Null;
			auto parse_state = Parse::Default;

			auto session = base_session_;
			auto&& sessionmap = getMap(root);
			
			//初始状态
			string line;

			//*** 主循环
			while (std::getline(inputFile, line)) { 

				//*reset pinfo status
				pinfo.buffer = "";
				pinfo.state = Parse::Default;
				pinfo.ct = Config_Type::Null;
				pinfo.is_mutlisession = false;
				pinfo.sevidx = 0;
				pinfo.value = "";

				//* parse line
				parseLine(line,parse_state);

				//* post-process if need
				if(pinfo.is_mutlisession){
					switch (pinfo.state){
						case Parse::Session:
							session = pinfo.buffer;
							try{
								sessionmap[session] = msc(Map());
							}catch(std::bad_variant_access e){
								TomlConfigLog(e.what());
							}
							break;
						case Parse::Vector_Session:
							session = pinfo.buffer;
							try{
								//表数组
								if(sessionmap.find(session) != sessionmap.end()){
									getVector(sessionmap[session]).emplace_back(msc(Map()));
								}else{
									sessionmap[session] = msc(Vector());
									getVector(sessionmap[session]).emplace_back(msc(Map()));
								}
							}catch(std::bad_variant_access e){
								TomlConfigLog(e.what());
							}
							break;
						case Parse::Value:
							if(session == base_session_){
								// sessionmap[pinfo.key] = msc(String(pinfo.value));
								auto tmpsessionmap = &sessionmap;
								for(int i=0;i<pinfo.sevidx-1;++i){
									auto ses = pinfo.sessionv[i];
									if(tmpsessionmap->find(ses) != tmpsessionmap->end()){
										tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
									}else{
										tmpsessionmap->operator[](ses) = msc(Map());
										tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
									}
								}
								switch (pinfo.ct){
									case Config_Type::Escape:
										[[fallthrough]];
									case Config_Type::String:
										tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(String(pinfo.value));
										break;
									case Config_Type::Int:
										tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Int(pinfo.value_i));
										break;
									case Config_Type::Bool:
										if( pinfo.value.find("true") ) {
											tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Bool(true));
										}else if( pinfo.value.find("false") ) {
											tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Bool(false));
										}else{
											TomlConfigLog("the bool config but not found true or false in value");
										}
										break;
									default:
										break;
								}
								break;
							}
							Map* tmpsessionmap;
							if(auto ptr = getIf<Vector>(sessionmap[session])){
								tmpsessionmap = &getMap(ptr->back());
							}else if(auto ptr = getIf<Map>(sessionmap[session])){
								tmpsessionmap = ptr;
							}	

							for(int i=0;i<pinfo.sevidx-1;++i){
								auto ses = pinfo.sessionv[i];
								if(tmpsessionmap->find(ses) != tmpsessionmap->end()){
									tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
								}else{
									tmpsessionmap->operator[](ses) = msc(Map());
									tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
								}
							}
							switch (pinfo.ct){
								case Config_Type::Escape:
									[[fallthrough]];
								case Config_Type::String:

									tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(String(pinfo.value));
									break;
								case Config_Type::Int:
									tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Int(pinfo.value_i));
									break;
								case Config_Type::Bool:
									if( pinfo.value.find("true") ) {
										tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Bool(true));
									}else if( pinfo.value.find("false") ) {
										tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Bool(false));
									}else{
										TomlConfigLog("the bool config but not found true or false in value");
									}
									break;
								default:
									break;
							}
							break;
						case Parse::Vector:{
							Map* tmpsessionmap;
							if(session == base_session_){
								tmpsessionmap = &sessionmap;
							}else{
								//* 是不是表数组
								if(auto ptr = getIf<Vector>(sessionmap[session])){
									tmpsessionmap = &getMap(ptr->back());
								}else if(auto ptr = getIf<Map>(sessionmap[session])){
									tmpsessionmap = ptr;
								}
							}
							//* 定位最后的表
							for(int i=0;i<pinfo.sevidx-1;++i){
								auto ses = pinfo.sessionv[i];
								if(tmpsessionmap->find(ses) != tmpsessionmap->end()){
									tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
								}else{
									tmpsessionmap->operator[](ses) = msc(Map());
									tmpsessionmap = &getMap(tmpsessionmap->operator[](ses));
								}
							}

							tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]) = msc(Vector());
							auto&& vec = getVector(tmpsessionmap->operator[](pinfo.sessionv[pinfo.sevidx-1]));
							for(int i=0;i<pinfo.vanyidx;i++){
								if ( pinfo.vany[i].type() == typeid(Int) ){
									vec.emplace_back(msc(Int(std::any_cast<Int>(pinfo.vany[i]))));
								}else if ( pinfo.vany[i].type() == typeid(String) ){
									vec.emplace_back(msc(String(std::any_cast<String>(pinfo.vany[i]))));
								}else if ( pinfo.vany[i].type() == typeid(Bool) ){
									vec.emplace_back(msc(Bool(std::any_cast<Bool>(pinfo.vany[i]))));
								}
							}																	
							break;
						}
						default:
							break;
					}
				}else{
					switch (pinfo.state){
						case Parse::Session:
							session = pinfo.buffer;
							try{
								sessionmap[session] = msc(Map());
							}catch(std::bad_variant_access e){
								TomlConfigLog(e.what());
							}
							break;
						case Parse::Vector_Session:
							session = pinfo.buffer;
							try{
								//表数组
								if(sessionmap.find(session) != sessionmap.end()){
									getVector(sessionmap[session]).emplace_back(msc(Map()));
								}else{
									sessionmap[session] = msc(Vector());
									getVector(sessionmap[session]).emplace_back(msc(Map()));
								}
							}catch(std::bad_variant_access e){
								TomlConfigLog(e.what());
							}
							break;
						case Parse::Comment:
							break;
						case Parse::Value:
							if(session == base_session_){
								switch (pinfo.ct){
									case Config_Type::Escape:
										[[fallthrough]];
									case Config_Type::String:
										sessionmap[pinfo.key] = msc(String(pinfo.value));									
										break;
									case Config_Type::Int:
										sessionmap[pinfo.key] = msc(Int(pinfo.value_i));							
										break;
									case Config_Type::Bool:
										if( pinfo.value.find("true") ) {
											sessionmap[pinfo.key] = msc(Bool(true));
										}else if( pinfo.value.find("false") ) {
											sessionmap[pinfo.key] = msc(Bool(false));
										}else{
											TomlConfigLog("the bool config but not found true or false in value");
										}
										break;
									default:
										break;
								}
								break;
							}
							switch (pinfo.ct){
								case Config_Type::Escape:
									[[fallthrough]];
								case Config_Type::String:
									if(auto ptr = getIf<Vector>(sessionmap[session])){
										getMap(ptr->back())[pinfo.key] = msc(String(pinfo.value));
										break;
									}else if(auto ptr = getIf<Map>(sessionmap[session])){
										ptr->operator[](pinfo.key) = msc(String(pinfo.value));
										break;
									}										
									break;
								case Config_Type::Int:
									if(auto ptr = getIf<Vector>(sessionmap[session])){
										getMap(ptr->back())[pinfo.key] = msc(Int(pinfo.value_i));
										break;
									}else if(auto ptr = getIf<Map>(sessionmap[session])){
										ptr->operator[](pinfo.key) = msc(Int(pinfo.value_i));
										break;
									}										
									break;
								case Config_Type::Bool:
									if(auto ptr = getIf<Vector>(sessionmap[session])){
										if( pinfo.value.find("true") ) {
											getMap(ptr->back())[pinfo.key] = msc(Bool(true));
										}else if( pinfo.value.find("false") ) {
											getMap(ptr->back())[pinfo.key] = msc(Bool(false));
										}else{
											TomlConfigLog("the bool config but not found true or false in value");
										}
									}else if(auto ptr = getIf<Map>(sessionmap[session])){
										if( pinfo.value.find("true") ) {
											ptr->operator[](pinfo.key) = msc(Bool(true));
										}else if( pinfo.value.find("false") ) {
											ptr->operator[](pinfo.key) = msc(Bool(false));
										}else{
											TomlConfigLog("the bool config but not found true or false in value");
										}
									}		
									break;
								default:
									break;
							}
							break;
						case Parse::Vector:{
							if(session == base_session_){
								sessionmap[pinfo.key] = msc(Vector());
								auto&& vec = getVector(sessionmap[pinfo.key]);
								for(int i=0;i<pinfo.vanyidx;i++){
									if ( pinfo.vany[i].type() == typeid(Int) ){
										vec.emplace_back(msc(Int(std::any_cast<Int>(pinfo.vany[i]))));
									}else if ( pinfo.vany[i].type() == typeid(String) ){
										vec.emplace_back(msc(String(std::any_cast<String>(pinfo.vany[i]))));
									}else if ( pinfo.vany[i].type() == typeid(Bool) ){
										vec.emplace_back(msc(Bool(std::any_cast<Bool>(pinfo.vany[i]))));
									}
								}
								break;
							}else{
								//* 是不是表数组
								if(auto ptr = getIf<Vector>(sessionmap[session])){
									auto&& map = getMap(ptr->back());
									map[pinfo.key] = msc(Vector());
									auto&& vec = getVector(map[pinfo.key]);
									for(int i=0;i<pinfo.vanyidx;i++){
										if ( pinfo.vany[i].type() == typeid(Int) ){
											vec.emplace_back(msc(Int(std::any_cast<Int>(pinfo.vany[i]))));
										}else if ( pinfo.vany[i].type() == typeid(String) ){
											vec.emplace_back(msc(String(std::any_cast<String>(pinfo.vany[i]))));
										}else if ( pinfo.vany[i].type() == typeid(Bool) ){
											vec.emplace_back(msc(Bool(std::any_cast<Bool>(pinfo.vany[i]))));
										}
									}
									break;
								}else if(auto ptr = getIf<Map>(sessionmap[session])){
									ptr->operator[](pinfo.key) = msc(Vector());
									auto&& vec = getVector(ptr->operator[](pinfo.key));
									for(int i=0;i<pinfo.vanyidx;i++){
										if ( pinfo.vany[i].type() == typeid(Int) ){
											vec.emplace_back(msc(Int(std::any_cast<Int>(pinfo.vany[i]))));
										}else if ( pinfo.vany[i].type() == typeid(String) ){
											vec.emplace_back(msc(String(std::any_cast<String>(pinfo.vany[i]))));
										}else if ( pinfo.vany[i].type() == typeid(Bool) ){
											vec.emplace_back(msc(Bool(std::any_cast<Bool>(pinfo.vany[i]))));
										}
									}
									break;
								}																		
							}
						}
						default:
							break;
					}
				}
			}

			//********************************************************end parse
			inputFile.close();
		} else {
			TomlConfigLog("Unable to open file" << path);
		}
	} else {
		TomlConfigLog("File not found" << path);
	}
}

inline configToml::config_ configToml::default_ = configToml::config_();
#define CONFIG configToml::getInstance()
}//namespace abcd
