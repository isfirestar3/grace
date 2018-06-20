#include "file_write_handler.h"
#include <share.h>
#include "log.h"

file_write_handler::file_write_handler(){

}

file_write_handler::~file_write_handler(){

}

int file_write_handler::create_file(const std::string& file_name, 
	const unsigned long file_size){
	if ((file_handler_ = fopen(file_name.c_str(), "wb+")) == NULL){
		nsperror << "failed to create bin file:" << file_name;
		return -1;
	}
	//写文件
	try{
		char* buffer = new char[file_size];
		memset(buffer, 0, file_size);
		if (fwrite(buffer, sizeof(char), file_size, file_handler_) != file_size){
			nsperror << "failed to init file.";
			fclose(file_handler_);
			delete[] buffer;
			return -1;
		}
		delete[] buffer;
	}
	catch (...){
		nsperror << "failed to allocate memory.";
		fclose(file_handler_);
		return -1;
	}
	//将指针挪至文件开头
	if (fseek(file_handler_, 0, SEEK_SET) != 0){
		nsperror << "failed to move file ptr to begin.";
		fclose(file_handler_);
		return -1;
	}
	return 0;
}

int file_write_handler::close_file(){
	return (file_handler_ != NULL) ? fclose(file_handler_) : -1;
}

int file_write_handler::write_file(const std::string& file_name, 
	const unsigned long offset, const std::string& data){
	//将指针挪至文件开头
	if (fseek(file_handler_, offset, SEEK_SET) != 0){
		nsperror << "failed to move file ptr to offset:" << (uint32_t)offset;
		return -1;
	}
	//写入数据
	if (fwrite(data.c_str(), sizeof(char), data.size(), file_handler_) != data.size()){
		nsperror << "failed to write block data to file.";
		return -1;
	}
	return 0;
}