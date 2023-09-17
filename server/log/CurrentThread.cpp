#include "CurrentThread.hpp"

#include <unistd.h>
#include <sys/syscall.h>

namespace tsuki::util::currentThread{
thread_local pthread_t t_id_cache_ = 0;
thread_local std::string t_name_cache_ = "default";
thread_local std::string t_id_string_cache_ = "(null)";

pthread_t this_thread_init_once(){
	if(__builtin_expect(t_id_cache_ == 0 ,0)){
		cacheid();
	}
	return t_id_cache_;
}
void cacheid(){
	t_id_cache_ = pthread_self();
	t_id_string_cache_ = std::to_string(t_id_cache_);
	if (t_id_string_cache_.length() > 6) {
    	t_id_string_cache_ = t_id_string_cache_.substr(t_id_string_cache_.length() - 6);
	}
	while (t_id_string_cache_.length() < 6) {
		t_id_string_cache_ += " ";
	}
	// t_id_string_cache_.erase(t_id_string_cache_.)
}
void setname(const std::string& s){
	t_name_cache_ = s;
	if (t_name_cache_.length() > 7) {
    	t_name_cache_ = t_name_cache_.substr(t_name_cache_.length() - 7);
	}
	while (t_name_cache_.length() < 7) {
		t_name_cache_ += " ";
	}
}
}