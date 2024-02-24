#include <iostream>
#include <cstring>

// struct{
// 	int is_set;
// 	char *str = const_cast<char*>(this->is_set == 777 ? this->str : "asdf");
// }test;

// __attribute__((constructor)) void start() {
// 	// test.is_set = 777;
// 	// test.str = new char[64];
// 	// char *s = "asd";
// 	// memmove(test.str, s, strlen(s)+1);
// }

int main(){
	char*str;
	char*sd= "sd";

	memmove(str, sd, 3);
}