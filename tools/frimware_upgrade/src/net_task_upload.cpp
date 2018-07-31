#include "net_task_upload.h"
#include "network_client_manager.h"

net_task_upload::net_task_upload(const std::string& local_ip, const std::string& ip, const unsigned int port,
	const FIRMWARE_SOFTWARE_TYPE type, const unsigned int file_length, const std::string& f_name)
	: ip_local_(local_ip), 
	ip_addr_(ip),
	port_(port), 
	f_type_(type), 
	firmware_length_(file_length), 
	file_name_(f_name){

}

net_task_upload::~net_task_upload(){

}

void net_task_upload::on_task(){
	uint64_t block_number = 1;
	if (firmware_length_ != 0){
		block_number = (firmware_length_ % FILE_BLOCK_SIZE == 0) ? (firmware_length_ / FILE_BLOCK_SIZE) : (firmware_length_ / FILE_BLOCK_SIZE + 1);
	}
	file_write_handler write_handler;
	if (write_handler.create_file(file_name_, firmware_length_) < 0){
		return;
	}
	char ip_port_addr[64];
	sprintf_s(ip_port_addr, sizeof(ip_port_addr), "%s:%d", ip_addr_.c_str(), port_);
	if (nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ip_local_,ip_port_addr) < 0){
		return;
	}
	for (size_t i = 0; i < block_number; i++){
		motion::asio_data asio_data_;
		nsp::os::waitable_handle water;
		recv_bin_data recv_data;
		//最后一次请求数据块大小不一定等于一个block块，所以此处需要分开处理
		int res = -1;
		if (i != (block_number - 1)){
			res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file(f_type_, ip_addr_, i*FILE_BLOCK_SIZE, FILE_BLOCK_SIZE, 
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
				recv_data = *(recv_bin_data*)data;
				water.sig();
			}));
		}
		else{
			res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file(f_type_, ip_addr_, i*FILE_BLOCK_SIZE, 
				firmware_length_ - i * FILE_BLOCK_SIZE, std::make_shared<motion::asio_block>([&](const void*data){
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
				function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, 0, nsp::proto::errorno_t::kUnsuccessful);
			}
			write_handler.close_file();
			return;
		}
		water.wait();
		if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
		{
			//判断写入是否成功，如果成功，则回调至主页面，不成功，直接返回
			if (write_handler.write_file(file_name_, recv_data.offset_, recv_data.data_context_) < 0){
				if (function_callback_){ 
					function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, -1, nsp::proto::errorno_t::kUnsuccessful); 
				}
				write_handler.close_file();
				return;
			}
			if (function_callback_)function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, ((double)(i+1) / (double)block_number) * 100, nsp::proto::errorno_t::kSuccessful);
		}
		else{
			//如果发现没有收到VCU的回包帧数据，则直接返回，不再发送下面的包
			if (function_callback_){
				function_callback_(ip_addr_, FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, -1, nsp::proto::errorno_t::kUnsuccessful);
			}
			write_handler.close_file();
			return;
		}
	}
	write_handler.close_file();
}

void net_task_upload::regiset_callback(const std::function<void(const std::string&, const int type,
	const int, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}