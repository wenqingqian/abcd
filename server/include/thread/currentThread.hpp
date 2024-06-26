#pragma once

#include <string>
#include <unistd.h>
#include <syscall.h>
#include <pthread.h>
namespace abcd::currentThread{

//注意thread_local开销
extern thread_local pthread_t t_id_cache_;
extern thread_local std::string t_name_cache_;
extern thread_local std::string t_id_string_cache_;

pthread_t this_thread_init_once();
void cacheid();
void setname(const std::string&);
inline pid_t gettid() { return static_cast<pid_t>(::syscall(SYS_gettid)); }
}//namespace abcd::currentThread
