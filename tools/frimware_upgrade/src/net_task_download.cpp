#include "net_task_download.h"
#include "log.h"
#include "file_read_handler.h"
#include "network_client_manager.h"
#include "toolkit.h"
#include "messageheader.h"
#include "basic_def.h"

net_task_download::net_task_download(const std::string& ip, const FIRMWARE_SOFTWARE_TYPE type, const uint32_t reset_timeout, const uint32_t block_delay, const uint32_t is_control)
	:ip_addr_(ip),
	f_type_(type),
	reset_timeout_(reset_timeout),
	reset_wait_(0),
	block_write_delay_(block_delay),
	block_write_wait_(0),
	is_control_(is_control){
}

net_task_download::net_task_download(const std::string& ip, const uint32_t reset_timeout, const uint32_t block_delay, uint8_t serial_id, uint8_t node_id)
:ip_addr_(ip),
reset_timeout_(reset_timeout),
reset_wait_(0),
block_write_delay_(block_delay),
block_write_wait_(0),
node_id_(node_id)
{
	serial_code_ = 0x00;
	serial_code_ += 0x0f && serial_id;
}


net_task_download::~net_task_download(){
	block_write_wait_.reset();
	reset_wait_.reset();
}

void net_task_download::on_task(){

	int block_num = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_block_num();
	int isInitial = 0;

	uint16_t check_sum = nsp::toolkit::singleton<file_read_handler>::instance()->get_check_sum();
	for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++)
	{
		if (initial_rom_update(block_num, check_sum) == 0)
		{
			isInitial = 1;
			break;
		}
	}

	if (isInitial == 0)
	{
		return ;
	}

	for (int i = 0; i < 3; i++){
		if (write_bin_file_to_vcu(block_num) < 0)continue;
		//下载成功，则从VCU车上再获取刚刚下发的bin文件，比对这两个文件是否相同，相同则表示文件一致
		/*if (get_bin_file(block_num) == 0){*/
			if (post_reset_request() == 0){
				//重启等待，等待VCU重启，后获取版本号
				reset_wait_.wait(reset_timeout_);
				//get_vcu_type_version_request();
			}
			break;
		//}
	}
}

int net_task_download::initial_rom_update( int block_num ,uint16_t check_sum )
{
	if (block_num <= 0)
	{
		return -1;
	}

	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;

	int res = nsp::toolkit::singleton<network_client_manager>::instance()->initial_rom_update(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
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
	}), serial_code_, block_num, node_id_,check_sum);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_initial_rom_update, "发送初始化指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_initial_rom_update, "初始化下载成功", nsp::proto::errorno_t::kUnsuccessful);
		return 0;
	}
	else{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_initial_rom_update, "初始化下载失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
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

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	for (size_t i = 0; i < block_number; i++){
		motion::asio_data asio_data_;
		nsp::os::waitable_handle water;
		recv_vcu_data recv_data;
		//最后一次请求数据块大小不一定等于一个block块，所以此处需要分开处理
		int res = -1;

		res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file_forward(
			ip_addr_, i, std::make_shared<motion::asio_block>([&](const void*data){
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
		}), serial_code_, node_id_);

		if (res < 0)
		{
			//发送不成功
			if (function_get_vcu_callback_){
				function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet_ex, "发送指令失败", nsp::proto::errorno_t::kUnsuccessful);
			}
			delete[] buffer_file;
			return -1;
		}
		water.wait();
		if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
		{
			//拷贝数据
			int nLength = recv_data.data_length_;
			nsp::proto::udp::can_data_t can_data;
			can_data.build((unsigned char *)recv_data.data_context_.data(), nLength);
			int offset = can_data.can_serial_index_0 << 8 + can_data.can_serial_index_1;

			memcpy_s(buffer_file + offset, sizeof(char)* can_data.data_.size(), can_data.data_.c_str(), can_data.data_.size());
		}
		else{
			//
			if (function_get_vcu_callback_){
				function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet_ex, "获取文件失败", nsp::proto::errorno_t::kUnsuccessful);
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
		if (function_get_vcu_callback_){
			function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet_ex, "文件校验失败", nsp::proto::errorno_t::kUnsuccessful);
		}
		delete[] buffer_file;
		return -1;
	}
	if (function_get_vcu_callback_){
		function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet_ex, "文件校验成功", nsp::proto::errorno_t::kSuccessful);
	}
	delete[] buffer_file;
	return 0;
}

void net_task_download::get_vcu_type_version_request(){
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	nsp::proto::udp::common_data recv_data_udp;
	recv_vcu_data recv_data;

	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_version_request(f_type_, ip_addr_, is_control_, std::make_shared<motion::asio_block>([&](const void*data){
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
	}), node_id_, kdevice_vcu_can, serial_code_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_get_software_version, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return ;
	}
	water.wait();

	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		//if (is_control_ == 0){
		//	int data;
		//	memcpy_s(&data, sizeof(data), recv_data.data_context_.c_str(), sizeof(data));
		//	char tmp[16];
		//	itoa(data,tmp,10);
		//	if (function_callback_)function_callback_(ip_addr_, operate_type, tmp, nsp::proto::errorno_t::kSuccessful);
		//	return 0;
		//}
		//else{
		//	if (function_callback_)function_callback_(ip_addr_, operate_type, recv_data.data_context_.c_str(), nsp::proto::errorno_t::kSuccessful);
		//	return 0;
		//}

		int nLength = recv_data.data_length_;
		recv_data_udp.build((unsigned char *)recv_data.data_context_.data(), nLength);
		nsp::proto::udp::can_data_t can_data;
		nLength = recv_data_udp.data_len_;
		can_data.build((unsigned char *)recv_data_udp.data_.data(), nLength);
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_get_software_version, can_data.data_.c_str(), nsp::proto::errorno_t::kSuccessful);
		return ;
	}
	else
	{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_get_software_version, "获取版本失败", nsp::proto::errorno_t::kUnsuccessful);
		return ;
	}
}

int net_task_download::write_bin_file_to_vcu(const int block_num){
	recv_vcu_data recv_data;

	for (size_t i = 0; i < block_num; i++){
		for (size_t j = 0; j < 3; j++){
			//发送写入VCU固件请求，发送一片数据片
			motion::asio_data asio_data_;
			nsp::os::waitable_handle water;
			size_t nIndex = i;
			int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_write_bin_file_fordward(ip_addr_, ++nIndex,
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

				recv_data = *(recv_vcu_data*)data;
			}), serial_code_, node_id_);

			const std::string &separator = LINE_SEPARATOR.toStdString();
			const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

			if (res < 0)
			{
				//发送不成功
				if (function_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet, "发送指令失败", nsp::proto::errorno_t::kUnsuccessful);
				//return -1;
				if (j == 2) return -1;
				else continue;
			}
			//超时等待
			water.wait();

			if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
			{
				int nLength = recv_data.data_length_;
				nsp::proto::udp::can_data_t can_data;
				can_data.build((unsigned char *)recv_data.data_context_.data(), nLength);

				uint16_t check_index = can_data.can_serial_index_1 + (can_data.can_serial_index_0 << 8);
				
				if (check_index == nIndex)
				{
					char buf[100] = {0};
					sprintf_s(buf, "%.0f%%", ((double)(nIndex) / (double)block_num) * 100);
					
					if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet, buf, nsp::proto::errorno_t::kSuccessful);
					//等待写入下一片的时间
					//block_write_wait_.wait(100);
				}
				else
				{
					if (j == 2)
					{
						if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet, "升级失败", nsp::proto::errorno_t::kUnsuccessful);
						return -1;
					}
					continue;
				}

				break;
			}
			else if (asio_data_.get_err() == nsp::proto::errorno_t::kSecurityFatal)
			{
				i = -1;
				break;
			}
			else{ 
				//如果发现没有收到VCU的回包帧数据，则直接返回，不再发送下面的包
				if (j == 2)
				{
					if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_download_packet, "升级失败", nsp::proto::errorno_t::kUnsuccessful);
					return -1;
				}
				continue;
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

	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_reset_request(ip_addr_,
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
	}), serial_code_, node_id_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		//发送不成功
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_reset, "发送重启指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_get_vcu_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_reset, "正在重启", nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_get_vcu_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_reset, "重启失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

void net_task_download::regiset_callback(const std::function<void(const std::string&, const int type, 
	const int, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}

void net_task_download::regiset_get_vcu_callback(const std::function<void(const std::string&, const int operate_type, const std::string&,
	const nsp::proto::errorno_t)>& func){
	if (func){
		function_get_vcu_callback_ = func;
	}
}