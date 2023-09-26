#ifndef ABCD_UTIL_CURRENTTHREAD_HPP
#define ABCD_UTIL_CURRENTTHREAD_HPP


#include <string>
#include <pthread.h>
namespace abcd::util::currentThread{

//注意thread_local开销
extern thread_local pthread_t t_id_cache_;
extern thread_local std::string t_name_cache_;
extern thread_local std::string t_id_string_cache_;

pthread_t this_thread_init_once();
void cacheid();
void setname(const std::string&);
}//namespace abcd

#endif