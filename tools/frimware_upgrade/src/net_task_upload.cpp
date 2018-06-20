#include "net_task_upload.h"
#include "network_client_manager.h"
#include "messageheader.h"
#include "basic_def.h"

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
	char ip_port_addr[64];
	char ip_port_local[64];
	sprintf_s(ip_port_addr, sizeof(ip_port_addr), "%s:%d", ip_addr_.c_str(), port_);
	sprintf_s(ip_port_local, sizeof(ip_port_local), "%s:%d", ip_local_.c_str(), 0);

	if (nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ip_port_local, ip_port_addr) < 0){
		return;
	}

	get_bin_file();
}

void net_task_upload::get_bin_file(){
	file_write_handler write_handler;
	if (write_handler.create_file(file_name_, firmware_length_) < 0){
		return;
	}

	uint64_t block_number = 1;
	if (firmware_length_ != 0){
		block_number = (firmware_length_ % FILE_BLOCK_SIZE == 0) ? (firmware_length_ / FILE_BLOCK_SIZE) : (firmware_length_ / FILE_BLOCK_SIZE + 1);
	}

	recv_vcu_data recv_data;
	uint8_t serial_code = 0x00;
	if (is_can_type_ == 0)
	{
		serial_code = 0x10;
	}
	serial_code += 0x0F && bas_serial_number_;

	for (size_t i = 0; i < block_number; i++){
		motion::asio_data asio_data_;
		nsp::os::waitable_handle water;
		//最后一次请求数据块大小不一定等于一个block块，所以此处需要分开处理
		int res = -1;
		/*if (i != (block_number - 1)){*/
		res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_read_bin_file_forward(ip_addr_, i,
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
			recv_data = *(recv_vcu_data*)data;
			water.sig();
		}), serial_code, node_id_);

		const std::string &separator = LINE_SEPARATOR.toStdString();
		const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

		if (res < 0)
		{
			//发送不成功
			if (function_callback_){
				function_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_upload_packet, "发送读取文件指令失败", nsp::proto::errorno_t::kUnsuccessful);
			}
			write_handler.close_file();
			return;
		}
		water.wait();
		if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
		{
			//判断写入是否成功，如果成功，则回调至主页面，不成功，直接返回
			nsp::proto::udp::can_data_t candata;
			int Length = recv_data.data_length_;
			candata.build((unsigned char *)recv_data.data_context_.data(), Length);
			int offset = candata.can_serial_index_0 << 8 + candata.can_serial_index_1;

			if (write_handler.write_file(file_name_, offset*FIRMWARE_WRITE_FILE_BLOCK_LENGTH, candata.data_) < 0){
				if (function_callback_){
					function_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_upload_packet, "写入文件失败", nsp::proto::errorno_t::kUnsuccessful);
				}
				write_handler.close_file();
				return;
			}

			char buf[100] = { 0 };
			sprintf_s(buf, "%.0f%%", ((double)(i) / (double)block_number) * 100);

			if (function_callback_)function_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_upload_packet, buf, nsp::proto::errorno_t::kSuccessful);
		}
		else{
			//如果发现没有收到VCU的回包帧数据，则直接返回，不再发送下面的包
			if (function_callback_){
				function_callback_(ipNodeIDStr, (int)kmessage_cmd_data_forward_upload_packet, "获取文件失败", nsp::proto::errorno_t::kUnsuccessful);
			}
			write_handler.close_file();
			return;
		}
	}
	write_handler.close_file();

	return;
}

void net_task_upload::regiset_callback(const std::function<void(const std::string&, const int type,
	const std::string& data, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}