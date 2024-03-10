#include "config.hpp"
#include "TomlConfig.hpp"
namespace abcd{

config conf;

using string = std::string;

void config::init(const char* path){
	char str[MIN_CONFIG_BUFF];

	printf("initialize tinymuduo\n");

	CONFIG.setFilepath(path);
	printf("server config path: %s\n", path);

	CONFIG_INT(CONFIG["server"]["port"], server.port)
	
	CONFIG_INT(CONFIG["server"]["thread_num"], server.thread_num)

	CONFIG_INT(CONFIG["log"]["flush_val"], log.flush_val)

	CONFIG_STRING(CONFIG["log"]["level"], log.level)

	CONFIG_STRING(CONFIG["log"]["output_path"], log.output_path)

	CONFIG_STRING(CONFIG["log"]["logHistoryRecordPath"], log.history_record_path)

	CONFIG_INT(CONFIG["log"]["buffer_vector_size"], log.buffer_vector_size)

	CONFIG_INT(CONFIG["log"]["buffer_vector_free_size"], log.buffer_vector_free_size)
	
	CONFIG_INT(CONFIG["log"]["buffer_to_write_max"], log.buffer_to_write_max)

	CONFIG_INT(CONFIG["log"]["roll_size"], log.roll_size)

	CONFIG_INT(CONFIG["log"]["time_check_interval"], log.time_check_interval)

	CONFIG_INT(CONFIG["log"]["log_store_limit"], log.log_store_limit)

	CONFIG_INT(CONFIG["server"]["maxfd"], server.maxfd)
	
	CONFIG.release();

	printf(R"(if not reset the config in %s, 
	it will be initialized between start_() and main(), 
	using the default value given in util/config.hpp,
	which print nothing here)""\n", path);
}

} // namespace abcd
