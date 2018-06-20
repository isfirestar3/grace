#include "file_read_handler.h"
#include "log.h"
#include "os_util.hpp"

file_read_handler::file_read_handler(){

}

file_read_handler::~file_read_handler(){

}

int file_read_handler::open_file(const std::string& file_name){
	file_size_ = nsp::os::get_filesize(file_name);
	if (file_size_ <= 0){
		return -1;
	}
	if ((file_handler_ = fopen(file_name.c_str(), "rb")) == NULL){
		loerror("agv_shell") << "failed to open bin file:" << file_name << ".";
		return -1;
	}
	//fseek函数将数据指针指向初始位置
	fseek(file_handler_, 0, SEEK_SET);
	//一次性读取出文件数据
	char* data = new char[file_size_];
	file_data_.clear();
	if (fread(data, sizeof(char), file_size_, file_handler_) != file_size_){
		loerror("agv_shell") << "failed to read bin file.";
		delete[] data;
		fclose(file_handler_);
		return -1;
	}
	file_data_.assign(data, file_size_);
	delete[] data;
	fclose(file_handler_);
	return 0;
}

//获取文件大小
uint64_t file_read_handler::get_file_size(){
	return file_size_;
}

//获取文件数据集
void file_read_handler::get_file_buffer(const uint32_t offset, const uint64_t read_size, std::string& buffer_data){
	if (offset + read_size > file_data_.size()){
		loerror("agv_shell") << "can not get file block data,the invaild parament.";
		return;
	}
	buffer_data.assign(file_data_.substr(offset, read_size).c_str(), read_size);
}