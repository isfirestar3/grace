#include "net_task_download.h"
#include "log.h"
#include "file_read_handler.h"
#include "network_client_manager.h"
#include "toolkit.h"

net_task_download::net_task_download(const std::string& ip, const FIRMWARE_SOFTWARE_TYPE type, const uint32_t reset_timeout, const uint32_t block_delay, const uint32_t is_control)
	:ip_addr_(ip),
	f_type_(type),
	reset_timeout_(reset_timeout),
	reset_wait_(0),
	block_write_delay_(block_delay),
	block_write_wait_(0),
	is_control_(is_control){
}

net_task_download::~net_task_download(){
	block_write_wait_.reset();
	reset_wait_.reset();
}

void net_task_download::on_task(){
	int block_num = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_block_num();

	for (int i = 0; i < 3; i++){
		if (write_bin_file_to_vcu(block_num) < 0)continue;
		//下载成功，则从VCU车上再获取刚刚下发的bin文件，比对这两个文件是否相同，相同则表示文件一致
		if (get_bin_file(block_num) == 0){
			if (post_reset_request() == 0){
				//重启等待，等待VCU重启，后获取版本号
				reset_wait_.wait(reset_timeout_);
				get_vcu_type_version_request();
			}
			break;
		}
	}
}

int net_task_download::get_bin_file(const int block_number){
	uint32_t file_size = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_size();
	char* buffer_file;
	try{
		buffer_file = new char[file_size];
	}
	catch (...){
		nsperror << "failed to new memory.";
		return -1;
	}
	for (size_t i = 0; i < block_number; i++){
		motion::asio_data asio_data_;
		nsp::os::waitable_handle water;
		recv_bin_data recv_data;
		//最后一次请求数据块大小不一定等于一个block块，所以此处需要分开处理
		int res = -1;
		if (i != (block_number - 1)){
			res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file(FIRMWARE_SOFTWARE_TYPE::COMPARE_SOFTWARE,
				ip_addr_, i*FILE_BLOCK_SIZE, FILE_BLOCK_SIZE,std::make_shared<motion::asio_block>([&](const void*data){
				if (!data){
					water.sig();
					return;
				}
				asio_data_ = *(motion::asio_data*)data;
				if (asio_data_.get_err() < 0){
					water.sig();
					return;
				}
				recv_data = *(recv_bin_data*)data;
				water.sig();
			}));
		}
		else{
			res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file(FIRMWARE_SOFTWARE_TYPE::COMPARE_SOFTWARE,
				ip_addr_, i*FILE_BLOCK_SIZE, file_size - i * FILE_BLOCK_SIZE, std::make_shared<motion::asio_block>([&](const void*data){
				if (!data){
					water.sig();
					return;
				}
				asio_data_ = *(motion::asio_data*)data;
				if (asio_data_.get_err() < 0){
					water.sig();
					return;
				}
				recv_data = *(recv_bin_data*)data;
				water.sig();
			}));
		}
		if (res < 0)
		{
			//发送不成功
			if (function_callback_){
				function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE, 0, nsp::proto::errorno_t::kUnsuccessful);
			}
			delete[] buffer_file;
			return -1;
		}
		water.wait();
		if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
		{
			//拷贝数据
			memcpy_s(buffer_file + recv_data.offset_, sizeof(char) * recv_data.data_context_.size(), recv_data.data_context_.c_str(), recv_data.data_context_.size());
		}
		else{
			//
			if (function_callback_){
				function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE, 0, nsp::proto::errorno_t::kUnsuccessful);
			}
			delete[] buffer_file;
			return -1;
		}
	}

	//md5比较两个文件
	unsigned char md5_vcu_file[16],md5_local_file[16];
	nsp::toolkit::md5<unsigned char>((unsigned char*)buffer_file, file_size, md5_vcu_file);
	std::string local_file;
	nsp::toolkit::singleton<file_read_handler>::instance()->get_file_buffer(local_file);
	nsp::toolkit::md5<unsigned char>((unsigned char*)local_file.c_str(), local_file.size(), md5_local_file);

	if (memcmp(md5_local_file, md5_vcu_file,16) != 0){
		if (function_callback_){
			function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE, 0, nsp::proto::errorno_t::kUnsuccessful);
		}
		delete[] buffer_file;
		return -1;
	}
	if (function_callback_){
		function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE, 0, nsp::proto::errorno_t::kSuccessful);
	}
	delete[] buffer_file;
	return 0;
}

void net_task_download::get_vcu_type_version_request(){
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	recv_vcu_data recv_data;
	//请求发送获取VCU固件版本信息
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_version_request(f_type_, ip_addr_,is_control_, std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)data;
		if (asio_data_.get_err() < 0){
			water.sig();
			return;
		}
		recv_data = *(recv_vcu_data*)data;
		water.sig();
	}));

	if (res < 0)
	{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return;
	}
	water.wait();

	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (is_control_ == 0){
			int data;
			memcpy_s(&data, sizeof(data), recv_data.data_context_.c_str(), sizeof(data));
			char tmp[16];
			itoa(data, tmp, 10);
			if (function_get_vcu_callback_)function_get_vcu_callback_(ip_addr_, FIRMWARE_COMPLETE_GET_VCU_VERSION, tmp, nsp::proto::errorno_t::kSuccessful);
		}
		else{
			if (function_get_vcu_callback_)function_get_vcu_callback_(ip_addr_, FIRMWARE_COMPLETE_GET_VCU_VERSION, recv_data.data_context_.c_str(), nsp::proto::errorno_t::kSuccessful);
		}

	}
	else{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kRequestTimeout);
	}
}

int net_task_download::write_bin_file_to_vcu(const int block_num){
	for (size_t i = 0; i < block_num; i++){
		for (size_t j = 0; j < 3; j++){
			//发送写入VCU固件请求，发送一片数据片
			motion::asio_data asio_data_;
			nsp::os::waitable_handle water;
			int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_write_bin_file(f_type_, ip_addr_, i*FILE_BLOCK_SIZE,
				nsp::toolkit::singleton<file_read_handler>::instance()->get_file_block_data(i),
				std::make_shared<motion::asio_block>([&](const void*data){
				if (!data){
					water.sig();
					return;
				}
				asio_data_ = *(motion::asio_data*)data;
				if (asio_data_.get_err() < 0){
					water.sig();
					return;
				}
				water.sig();
			}));

			if (res < 0)
			{
				//发送不成功
				if (function_callback_)function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, 0, nsp::proto::errorno_t::kUnsuccessful);
				//return -1;
				if (j == 2) return -1;
				else continue;
			}
			//超时等待
			water.wait();

			if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
			{
				if (function_callback_)function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, ((double)(i + 1) / (double)block_num) * 100, nsp::proto::errorno_t::kSuccessful);
				//等待写入下一片的时间
				block_write_wait_.wait(100);
				//continue;
				break;
			}
			else{
				//如果发现没有收到VCU的回包帧数据，则直接返回，不再发送下面的包
				if (function_callback_)function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, ((double)(i + 1) / (double)block_num) * 100, nsp::proto::errorno_t::kUnsuccessful);
				if (j == 2) return -1;
				else continue;
				//return -1;
			}
		}

	}
	return 0;
}

int net_task_download::post_reset_request(){
	//发送重启信号
	uint32_t file_size = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_size();
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_reset_request(ip_addr_, file_size,
		std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)data;
		if (asio_data_.get_err() < 0){
			water.sig();
			return;
		}
		water.sig();
	}));

	if (res < 0)
	{
		//发送不成功
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 100, nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

void net_task_download::regiset_callback(const std::function<void(const std::string&, const int type, 
	const int, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}

void net_task_download::regiset_get_vcu_callback(const std::function<void(const std::string&, const int type, const std::string&, 
	const nsp::proto::errorno_t)>& func){
	if (func){
		function_get_vcu_callback_ = func;
	}
}