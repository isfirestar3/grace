#include "file_read_handler.h"
#include "log.h"

file_read_handler::file_read_handler(){

}

file_read_handler::~file_read_handler(){

}

int file_read_handler::open_file(const std::string& file_name){
	if ((file_handler_ = _fsopen(file_name.c_str(), "rb", _SH_DENYWR)) == NULL){
		nsperror << "failed to open bin file:" << file_name;
		return -1;
	}
	//获取文件大小，此处未使用fseek将指针挪至文件尾，尝试使用新方法读取文件
	struct _stat32 buf;
	if (_stat32(file_name.c_str(), &buf) != 0){
		nsperror << "failed to get bin file size.";
		fclose(file_handler_);
		return -1;
	}

	uint64_t block_number = 1;
	file_size_ = buf.st_size;
	if (buf.st_size != 0){
		block_number = (buf.st_size % FILE_BLOCK_SIZE == 0) ? (buf.st_size / FILE_BLOCK_SIZE) : (buf.st_size / FILE_BLOCK_SIZE + 1);
	}
	map_file_block_.clear();
	//fseek函数将数据指针指向初始位置
	fseek(file_handler_, 0, SEEK_SET);
	for (int i = 0; i < block_number; i++){
		if (i != (block_number - 1)){
			char line_block[FILE_BLOCK_SIZE] = {0};
			if (fread(line_block, sizeof(char), FILE_BLOCK_SIZE, file_handler_) != FILE_BLOCK_SIZE){
				nsperror << "failed to read bin file while the block is:" << i;
				return -1;
			}
			std::string tmp_data;
			tmp_data.assign(line_block, sizeof(line_block));
			map_file_block_.insert(std::make_pair(i, tmp_data));
		}
		else{
			//最后一片数据，由于数据大小未知，故此处另做处理
			uint64_t end_size = buf.st_size - i*FILE_BLOCK_SIZE;
			char* line_block = new char[end_size];
			if (fread(line_block, sizeof(char), end_size, file_handler_) != end_size){
				nsperror << "failed to read bin file in the last block.";
				return -1;
			}
			std::string tmp_data;
			tmp_data.assign(line_block, end_size);
			map_file_block_.insert(std::make_pair(i, tmp_data));
			delete[] line_block;
		}
	}
	fclose(file_handler_);
	return 0;
}

//关闭文件句柄
int file_read_handler::close_file(){
	if (file_handler_)return fclose(file_handler_);
	return -1;
}

//获取文件块数量，map集合中key值由第0块递增
int file_read_handler::get_file_block_num(){
	return map_file_block_.size();
}

//根据索引号获取数据块
std::string file_read_handler::get_file_block_data(const int index){
	std::lock_guard<decltype(res_mutex_)> lock(res_mutex_);
	auto iter = map_file_block_.find(index);
	if (iter == map_file_block_.end()){
		nspwarn << "cannot find file block number : " << index << " data.";
		return NULL;
	}
	return iter->second;
}

//获取文件大小
uint32_t file_read_handler::get_file_size(){
	return file_size_;
}

//获取文件数据集
int file_read_handler::get_file_buffer(std::string& buffer_data){
	for (size_t i = 0; i < map_file_block_.size(); i++){
		auto iter = map_file_block_.find(i);
		if (iter == map_file_block_.end()){
			return -1;
		}
		buffer_data.append(iter->second.substr(0, iter->second.size()));
	}
	return 0;
}