#define MIN_CONFIG_BUFF 64
#define CONFIG_IS_SET_MAGIC 777

#define CONFIG_IS_SET_NAME(name) name ## _IsSeT

#define CONFIG_SET_NUM(type, name, default) \
	type name = (this->CONFIG_IS_SET_NAME(name) == CONFIG_IS_SET_MAGIC ? this->name : default); \
	int CONFIG_IS_SET_NAME(name);
	
#define CONFIG_SET_VEC(type, name, default) \
	type *name = const_cast<type*>(this->CONFIG_IS_SET_NAME(name) == CONFIG_IS_SET_MAGIC ? this->name : default); \
	int CONFIG_IS_SET_NAME(name);

#define CONFIG_STRING(from, to) \
	if(from.value<string>(str)){ \
		to = new char[MIN_CONFIG_BUFF]; \
		memmove(to, str, strlen(str)+1); \
		CONFIG_IS_SET_NAME(to) = CONFIG_IS_SET_MAGIC; \
		printf( #to": %s\n", to); \
	}else{ \
		printf( #to": (null)\n"); \
	}

#define CONFIG_INT(from, to) \
	if(auto option = from.value<int>()){ \
		to = *option; \
		CONFIG_IS_SET_NAME(to) = CONFIG_IS_SET_MAGIC; \
		printf( #to": %d\n", to); \
	}else{ \
		printf( #to": (null)\n"); \
	}

