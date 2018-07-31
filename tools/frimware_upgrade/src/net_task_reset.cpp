#include "net_task_reset.h"
#include "log.h"
#include "file_read_handler.h"
#include "network_client_manager.h"
#include "toolkit.h"

net_task_reset::net_task_reset(){

}

net_task_reset::~net_task_reset(){
	
}

int net_task_reset::net_reset(std::string& ip_addr)
{
	//发送重启信号
	uint32_t file_size = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_size();
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;

	uint8_t serial_code = 0x00;
	if (is_can_type_ == 0)
	{
		serial_code = 0x10;
	}
	serial_code += 0x0F && bas_serial_number_;

	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_reset_request(ip_addr,
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
	}),serial_code,node_id_);

	if (res < 0)
	{
		//发送不成功
		if (function_callback_)function_callback_(ip_addr, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ip_addr, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 100, nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ip_addr, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

void net_task_reset::regiset_callback(const std::function<void(const std::string&, const int type,
	const int, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}


void net_task_reset::get_vcu_type_version_request(FIRMWARE_SOFTWARE_TYPE type, std::string& ip_addr,int is_control){
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	recv_vcu_data recv_data;
	//请求发送获取VCU固件版本信息
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_version_request(type, ip_addr, is_control, std::make_shared<motion::asio_block>([&](const void*data){
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
		if (function_callback_)function_callback_(ip_addr, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kUnsuccessful);
		return;
	}
	water.wait();

	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		operate_code operate_type;
		operate_type.cmd_code = FIRMWARE_COMPLETE_GET_VCU_VERSION;

		if (is_control == 0){
			int data;
			memcpy_s(&data, sizeof(data), recv_data.data_context_.c_str(), sizeof(data));
			char tmp[16];
			itoa(data, tmp, 10);
			if (function_get_vcu_callback_)function_get_vcu_callback_(ip_addr, operate_type, tmp, nsp::proto::errorno_t::kSuccessful);
		}
		else{
			if (function_get_vcu_callback_)function_get_vcu_callback_(ip_addr, operate_type, recv_data.data_context_.c_str(), nsp::proto::errorno_t::kSuccessful);
		}

	}
	else{
		if (function_callback_)function_callback_(ip_addr, FIRMWARE_RESET_OPERATE_CODE_CALLBACK, 0, nsp::proto::errorno_t::kRequestTimeout);
	}
}

void net_task_reset::regiset_get_vcu_callback(const std::function<void(const std::string&, const operate_code operate_type, const std::string&,
	const nsp::proto::errorno_t)>& func){
	if (func){
		function_get_vcu_callback_ = func;
	}
}