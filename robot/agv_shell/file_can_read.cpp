#include "file_can_read.h"
#include "const.h"
#include "log.h"

file_can_read::file_can_read()
	:file_block_size_(FILE_CAN_BLOCK_SIZE_CAN)
{

}

file_can_read::~file_can_read(){

}

int file_can_read::open_file(const std::string& file_name){
	if ((file_handler_ = fopen(file_name.c_str(), "rb")) == NULL){
		nsperror << "failed to open bin file:" << file_name;
		return -1;
	}
	//获取文件大小，此处未使用fseek将指针挪至文件尾，尝试使用新方法读取文件
	/*struct _stat32 buf;
	if (_stat32(file_name.c_str(), &buf) != 0){
		nsperror << "failed to get bin file size.";
		fclose(file_handler_);
		return -1;
	}*/

	fseek(file_handler_, 0, SEEK_SET);
	std::string str_file_data;
	char chbuf[1024] = { 0 };
	uint32_t nRead = 0;
	uint32_t nOffset = 0;

	while ((nRead = fread(chbuf, sizeof(char), sizeof(chbuf), file_handler_)) > 0)
	{
		nOffset = 0;
		while (nOffset< nRead)
		{
			if (chbuf[nOffset] != '\r' && chbuf[nOffset] != '\n')
			{
				str_file_data.append(chbuf+nOffset, 1);
			}
			nOffset++;
		}

		memset(chbuf, 0, sizeof(chbuf));
	}
	fclose(file_handler_);
	map_file_block_.clear();

	nOffset = 0;
	while (nOffset < str_file_data.size() / file_block_size_)
	{
		std::string tmp_data = "";
		tmp_data.assign( &str_file_data[nOffset *file_block_size_], file_block_size_);
		map_file_block_.insert(std::make_pair(nOffset, tmp_data));
		nOffset++;
	}
	
	int32_t last_nOffset = str_file_data.size() % file_block_size_;

	if (last_nOffset > 0)
	{
		std::string tmp_data = "";
		tmp_data.assign(&str_file_data[nOffset *file_block_size_], last_nOffset);
		map_file_block_.insert(std::make_pair(nOffset, tmp_data));
	}
	return 0;
}

//关闭文件句柄
int file_can_read::close_file(){
	if (file_handler_)return fclose(file_handler_);
	return -1;
}

//获取文件块数量，map集合中key值由第0块递增
int file_can_read::get_file_block_num(){
	return map_file_block_.size();
}

void file_can_read::set_block_size(uint32_t block_size)
{
	if ( block_size == 0 )
	{
		block_size = FILE_CAN_BLOCK_SIZE;
	}

	file_block_size_ = block_size;
}

//根据索引号获取数据块
std::string file_can_read::get_file_block_data(const int index){
	std::lock_guard<decltype(res_mutex_)> lock(res_mutex_);
	auto iter = map_file_block_.find(index);
	if (iter == map_file_block_.end()){
		nspwarn << "cannot find file block number : " << index << " data.";
		return NULL;
	}
	return iter->second;
}

//获取文件大小
uint32_t file_can_read::get_file_size(){
	return file_size_;
}

//获取文件数据集
int file_can_read::get_file_buffer(std::string& buffer_data){
	for (size_t i = 0; i < map_file_block_.size(); i++){
		auto iter = map_file_block_.find(i);
		if (iter == map_file_block_.end()){
			return -1;
		}
		buffer_data.append(iter->second.substr(0, iter->second.size()));
	}
	return 0;
}