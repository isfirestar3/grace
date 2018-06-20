#include "file_read_handler.h"
#include "log.h"

file_read_handler::file_read_handler()
:file_block_size_(FILE_BLOCK_SIZE_CAN)
{

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

	fseek(file_handler_, 0, SEEK_SET);
	std::string str_file_data;
	char chbuf[1024] = { 0 };
	int32_t  nRead = 0;
	int32_t nOffset = 0;
	bool isHexFile = true;
	if (file_name.find(".bin") != std::string::npos){
		isHexFile = false;
	}

	while ((nRead = fread(chbuf, sizeof(char), sizeof(chbuf), file_handler_)) > 0)
	{
		nOffset = 0;
		while (nOffset< nRead)
		{
			if (isHexFile == false || (chbuf[nOffset] != '\r' && chbuf[nOffset] != '\n')){
				str_file_data.append(chbuf + nOffset, 1);
			}
			nOffset++;
		}

		memset(chbuf, 0, sizeof(chbuf));
	}
	fclose(file_handler_);
	map_file_block_.clear();

	nOffset = 0;
	check_sum_ = 0;
	while (nOffset < str_file_data.size() / file_block_size_)
	{
		std::string tmp_data = "";
		tmp_data.assign( &str_file_data[nOffset *file_block_size_], file_block_size_);
		map_file_block_.insert(std::make_pair(nOffset, tmp_data));
		nOffset++;
		check_sum(tmp_data);
	}

	int32_t nlast = str_file_data.size() % file_block_size_;
	if (nlast > 0)
	{
		std::string tmp_data = "";
		tmp_data.assign(&str_file_data[nOffset *file_block_size_], nlast);
		map_file_block_.insert(std::make_pair(nOffset, tmp_data));
		check_sum(tmp_data);
	}
	return 0;
}

void file_read_handler::check_sum(std::string buffer )
{
	for (int index = 0; index < buffer.length(); index++){
		if (0 == index % 2 ){
			check_sum_ += uint8_t(buffer[index]);
		}
		else{
			check_sum_ += uint8_t(buffer[index]) << 8;
			if (check_sum_ == 0x4f2b){
				int j = 5;
			}
		}
	}
	//static uint16_t sum = 0;
	//for (int index = 0; index < buffer.length();){
	//	uint16_t temp = 0;
	//	int size = 1;
	//	if (buffer.length() - index > 2){
	//		size = 2;
	//	}

	//	memcpy(&temp, buffer.c_str() + index, size);
	//	sum += temp;
	//	index = index + 2;
	//}
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

void file_read_handler::set_block_size(uint32_t block_size)
{
	if ( block_size == 0 )
	{
		block_size = FILE_BLOCK_SIZE;
	}

	file_block_size_ = block_size;
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