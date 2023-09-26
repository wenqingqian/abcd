#ifndef ABCD_NONCOPTABLE_HPP
#define ABCD_NONCOPTABLE_HPP

class noncopyable{
public:
	noncopyable(const noncopyable &) = delete;
	const noncopyable &operator=(const noncopyable &) = delete;
protected:
	noncopyable() {}
	~noncopyable() {}
};

#endif