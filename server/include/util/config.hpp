#pragma once
#include "common.hpp"
#include "config_define.hpp"
namespace abcd {

// 用toml解析完设置在这, 代码里不用CONFIG, 简洁点
class config : public noncopyable{
public:
	config(){}
	~config(){}
	
	void init(const char*);
	
	struct{
		//         type, name, default_value
		CONFIG_SET_NUM(int, port, 8002)
		CONFIG_SET_NUM(int, thread_num, 4)
		CONFIG_SET_NUM(int, maxfd, 100000)
	}server;

	struct{
		//         type, name, default_value
		CONFIG_SET_NUM(int, roll_size, 20*1024*1024)
		CONFIG_SET_NUM(int64_t, log_store_limit, 4LL*1024*1024*1024)
		CONFIG_SET_NUM(int, time_check_interval, (60/3)*60*4)

		CONFIG_SET_VEC(char, level, "DEFAULT")
		CONFIG_SET_NUM(int, flush_val, 3)
		CONFIG_SET_NUM(int, buffer_vector_size, 8)
		CONFIG_SET_NUM(int, buffer_vector_free_size, 4)
		CONFIG_SET_NUM(int, buffer_to_write_max, 16)
		CONFIG_SET_VEC(char, output_path, "log/abcd.log")
		CONFIG_SET_VEC(char, history_record_path, "log/logHistoryRecord.txt")
	}log;
};

extern config conf;
} // namespace abcd
