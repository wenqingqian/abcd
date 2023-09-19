#include "noncopyable.hpp"
#include <fstream>
#include <string>
#include <filesystem>

namespace tsuki::util{
namespace fs = std::filesystem;

class File : public noncopyable{
public:
	File(){}
	File(const std::string& str, std::ios_base::openmode mode = std::ios::out)
		:	basename_(str),
			extensionname_(basename_.extension()),
			stemname_(basename_.stem()),
			dirname_(basename_.parent_path())
			{
		ofs_.open(str,mode);
		filesize_ = ofs_.tellp();
	}
	~File(){ ofs_.flush();ofs_.close(); }
	void reset(const std::string& str, std::ios_base::openmode mode = std::ios::out){
		if(ofs_.is_open()){
			ofs_.flush();ofs_.close();
		}
		ofs_.open(str,mode);
		basename_ = str;
		extensionname_ = basename_.extension();
		stemname_ = basename_.stem();
		dirname_  = basename_.parent_path();
		filesize_ = ofs_.tellp();
	}

	void flush() { ofs_.flush(); }
	void close() { ofs_.close(); }
	int64_t write(const char* data, int len){
		auto pos = getFileSize();
		ofs_.write(data,len);
		filesize_ = ofs_.tellp();
		return filesize_ - pos;
	}

	std::string getStemName() { return stemname_.string(); }
	std::string getDirName () { return  dirname_.string(); }
	std::string getFileName() { return basename_.string(); }
	std::string getExtension(){ return extensionname_.string(); }
	

	int64_t getFileSize() { return filesize_; }
private:
	//基础输入路径名
	fs::path basename_;
	int64_t filesize_ = 0;
	std::ofstream ofs_;
	//文件名, 无扩展
	fs::path stemname_;
	//文件目录名
	fs::path dirname_;
	//文件类型扩展
	fs::path extensionname_;
	// std::string typeextension_;
};

} // namespace tsuki::util
