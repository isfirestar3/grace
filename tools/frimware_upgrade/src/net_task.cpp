#include "net_task.h"
#include "network_client_manager.h"
#include "basic_def.h"

net_task::net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, const FIRMWARE_SOFTWARE_TYPE type, int is_control, uint8_t node_id, enum  device_type type_device)
	:ip_local_(local_ip), 
	ip_addr_(ip), 
	port_(port), 
	f_type_(type),
	is_control_(is_control),
	node_id_(node_id),
	kdevice_type_(type_device),
	serial_code_(0x00),
	is_can_type_(1){
}

net_task::net_task(const std::string& local_ip, const std::string& ip, const unsigned int port, enum message_type messagetype, uint8_t serial_id, uint8_t node_id, uint8_t register_index, int is_upload)
:ip_local_(local_ip),
ip_addr_(ip),
port_(port),
node_id_(node_id),
messagetype_(messagetype),
index_register_(register_index),
is_upload_(is_upload)
{
	serial_code_ = 0x00;
	serial_code_ += 0x0f && serial_id;
	is_control_ = 0;
	kdevice_type_ = kdevice_vcu_can;
	is_can_type_ = 0;
}

net_task::~net_task(){
}

void net_task::on_task(){
	//创建
	char ip_port_addr[64];
	char ip_port_local[64];
	sprintf_s(ip_port_addr, sizeof(ip_port_addr), "%s:%d", ip_addr_.c_str(), port_);
	sprintf_s(ip_port_local, sizeof(ip_port_local), "%s:%d", ip_local_.c_str(), 5057);

	if (nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ip_port_local, ip_port_addr) < 0){
		return;
	}
	//在未请求成功的情况下，做3次请求操作

	switch (messagetype_ )
	{
	case kmessage_cmd_start_data_forward:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++){
			if (start_data_foward() == 0)break;
		}
		break;
	case kmessage_cmd_data_forward_get_hardware_type:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++){
			if (get_vcu_type() == 0)break;
		}
		break;
	case kmessage_cmd_data_forward_get_software_version:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++){
			if (get_vcu_version() == 0)break;
		}
		break;
	case kmessage_cmd_data_forward_read_register:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++)
		{
			if (read_register_data() == 0)
			{
				break;
			}
		}
		break;
	case kmessage_cmd_data_forward_write_register:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++)
		{
			if (write_register_data() == 0)
			{
				break;
			}
		}
		break;
	case kmessage_cmd_data_forward_reset:
		for (int index = 0; index < FIRMWARE_RETURY_NUMBER; index++)
		{
			if (write_register_data() == 0)
			{
				break;
			}
		}
	default:
		break;
	}
}

bool net_task::init_net()
{
	char ip_port_addr[64];
	char ip_port_local[64];
	sprintf_s(ip_port_addr, sizeof(ip_port_addr), "%s:%d", ip_addr_.c_str(), port_);
	sprintf_s(ip_port_local, sizeof(ip_port_local), "%s:%d", ip_local_.c_str(), 5057);

	if (nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ip_port_local, ip_port_addr) < 0){
		return false;
	}

	return true;
}

int net_task::start_data_foward()
{
	if (!init_net())
	{
		return -1;
	}

	//发送获取VCU类型请求
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->start_data_foward(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
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
	}), serial_code_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "发送指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "启动转发功能成功", nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "启动转发功能失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::read_register_data()
{
	//发送获取VCU类型请求
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	nsp::proto::udp::common_data recv_data_udp;
	recv_vcu_data recv_data;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)data;
		if (asio_data_.get_err() < 0){
			water.sig();
			return;
		}
		recv_data= *(recv_vcu_data*)data;
		water.sig();
	}), serial_code_,index_register_,node_id_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "发送指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "", nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "读取寄存器失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::write_register_data()
{
	//发送获取VCU类型请求
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	nsp::proto::udp::common_data recv_data;
	int res = nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
		if (!data){
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)data;
		if (asio_data_.get_err() < 0){
			water.sig();
			return;
		}
		recv_data = *(nsp::proto::udp::common_data*)data;
		water.sig();
	}), serial_code_, index_register_, node_id_,"");

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "发送指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "", nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "写寄存器失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::get_vcu_type(){
	if (!init_net())
	{
		return -1;
	}

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
	}), node_id_, kdevice_type_, serial_code_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		int nLength = recv_data.data_length_;
		nsp::proto::udp::can_data_t can_data;
		can_data.build((unsigned char *)recv_data.data_context_.data(), nLength);
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, can_data.data_, nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::get_vcu_version(){
	if (!init_net())
	{
		return -1;
	}

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
	}), node_id_, kdevice_type_, serial_code_);

	const std::string &separator = LINE_SEPARATOR.toStdString();
	const std::string &ipNodeIDStr = ip_addr_ + separator + QString::number(node_id_).toStdString();

	if (res < 0)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
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
		nsp::proto::udp::can_data_t can_data;
		can_data.build((unsigned char *)recv_data.data_context_.data(), nLength);
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, can_data.data_.c_str(), nsp::proto::errorno_t::kSuccessful);
		return 0;

	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

int net_task::net_reset()
{
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
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "发送重启指令失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
	water.wait();
	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful)
	{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "正在重启", nsp::proto::errorno_t::kSuccessful);
		return 0;
	}
	else{
		if (function_callback_)function_callback_(ipNodeIDStr, (int)messagetype_, "重启失败", nsp::proto::errorno_t::kUnsuccessful);
		return -1;
	}
}

//int net_task::get_vcu_cpu(){
//	//请求发送获取CPU版本信息
//	motion::asio_data asio_data_;
//	nsp::os::waitable_handle water;
//	recv_vcu_data recv_data;
//	int res = nsp::toolkit::singleton<network_client_manager>::instance()->post_cpu_type_requst(ip_addr_, std::make_shared<motion::asio_block>([&](const void*data){
//		if (!data){
//			water.sig();
//			return;
//		}
//		asio_data_ = *(motion::asio_data*)data;
//		if (asio_data_.get_err() < 0){
//			water.sig();
//			return;
//		}
//		recv_data = *(recv_vcu_data*)data;
//		water.sig();
//	}));
//
//	if (res < 0){
//		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
//		return -1;
//	}
//	water.wait();
//	if (asio_data_.get_err() == nsp::proto::errorno_t::kSuccessful) {
//		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, recv_data.data_context_, nsp::proto::errorno_t::kSuccessful);
//		return 0;
//	}
//	else {
//		if (function_callback_) function_callback_(ip_addr_, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION, "无法获取", nsp::proto::errorno_t::kUnsuccessful);
//		return -1;
//	}
//}

void net_task::regiset_callback(const std::function<void(const std::string&, const int operate_type, const std::string&, const nsp::proto::errorno_t)>& func){
	if (func){
		function_callback_ = func;
	}
}