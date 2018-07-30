#include "net_task.h"
#include "network_client_manager.h"

net_task::net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, const FIRMWARE_SOFTWARE_TYPE type, int is_control)
	:ip_local_(local_ip), 
	ip_addr_(ip), 
	port_(port), 
	f_type_(type),
	is_control_(is_control){
}

net_task::~net_task(){
}

void net_task::on_task(){
	//创建
	char ip_port_addr[64];
	sprintf_s(ip_port_addr, sizeof(ip_port_addr), "%s:%d", ip_addr_.c_str(), port_);
	if (nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ip_local_,ip_port_addr) < 0){
		return;
	}

	//在未请求成功的情况下，做3次请求操作
	for (int i = 0; i < 3; i++){
		if (get_vcu_type() == 0)break;
	}
	
	for (int i = 0; i < 3; i++){
		if (get_vcu_version() == 0)break;
	}

	for (int i = 0; i < 3; i++){
		if (get_vcu_cpu() == 0)break;
	}
}

int net_task::get_vcu_type(){
	//发送获取VCU类型请求
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	recv_vcu_data recv_data;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_vcu_type_request(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
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
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE, recv_data.data_context_, nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::get_vcu_version(){
	//请求发送获取VCU固件版本信息
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
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
	}));

	if (res < 0)
	{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();

	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (is_control_ == 0){
			int data;
			memcpy_s(&data, sizeof(data), recv_data.data_context_.c_str(), sizeof(data));
			char tmp[16];
			itoa(data,tmp,10);
			if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION, tmp, nsp::proto::errorno_t::kSuccessful);
			return 0;
		}
		else{
			if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION, recv_data.data_context_.c_str(), nsp::proto::errorno_t::kSuccessful);
			return 0;
		}

	}
	else{
		if (function_callback_)function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::get_vcu_cpu(){
	//请求发送获取CPU版本信息
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	recv_vcu_data recv_data;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_cpu_type_requst(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
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
	if (res < 0){
		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful) {
		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, recv_data.data_context_, nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else {
		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

void net_task::regiset_callback(const std::function<void(const std::string&, const int type, const std::string&, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}