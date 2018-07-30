#include "network_session.h"
#include "file_read_handler.h"

network_session::network_session(){

}

network_session::~network_session(){

}

void network_session::on_recvdata(const std::string &data, const nsp::tcpip::endpoint &r_ep){
	nsp::proto::udp::unpackage upkt(data);
	switch (upkt.ack_type())
	{
	case EXECUTE_ACK::ACK_Normal:
		break;
	case EXECUTE_ACK::ACK_Version_Failed:
	case EXECUTE_ACK::ACK_Limits:
	case EXECUTE_ACK::ACK_Command_No:
	case EXECUTE_ACK::ACK_Addr_Not_Write:
	case EXECUTE_ACK::ACK_OverFlow_Memory:
	case EXECUTE_ACK::ACK_Content_Invaild:
	case EXECUTE_ACK::ACK_VCU_Busy:
	case EXECUTE_ACK::ACK_Target_Port_Faild:
	case EXECUTE_ACK::ACK_Target_Not_exist:
	case EXECUTE_ACK::ACK_Target_Not_Reponse:
	case EXECUTE_ACK::ACK_Data_Lose:
	case EXECUTE_ACK::ACK_Other_Problom:
		return;
	default:
		break;
	}
	switch (upkt.main_operate())
	{
	case FIRMWARE_MAIN_OPERATE_CODE_MODULE_CMD:
		dispatch_vcu_recv_info(upkt.pkt_id(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
		break;
	case FIRMWARE_MAIN_OPERATE_CODE_GET_INFO:
		dispatch_vcu_get_info(upkt.pkt_id(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
		break;
	case FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE:
		dispatch_vcu_rw_firmware(upkt.pkt_id(), upkt.sub_operate(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
		break;
	default:
		break;
	}
}

void network_session::dispatch_vcu_recv_info(const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep){
	recv_vcu_data data;
	data.set_err(nsp::proto::kSuccessful);
	data.remote_ip_ = r_ep.ipv4();
	data.data_length_ = pkt->data_len_;
	data.data_context_ = pkt->data_;
	nsp::toolkit::singleton<net_manager>::instance()->exec(pkt_id, (char *)&data);
	return;
}

void network_session::dispatch_vcu_get_info(const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep){
	recv_vcu_data data;
	data.set_err(nsp::proto::kSuccessful);
	data.remote_ip_ = r_ep.ipv4();
	data.data_length_ = pkt->data_len_;
	data.data_context_ = pkt->data_;
	nsp::toolkit::singleton<net_manager>::instance()->exec(pkt_id, (char *)&data);
	return;
}

void network_session::dispatch_vcu_rw_firmware(const uint16_t pkt_id, const short int sub_operate, 
	const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep){
	if (sub_operate == FIRMWARE_SUB_OPERATE_CODE_READ_BOOT_FILE || sub_operate == FIRMWARE_SUB_OPERATE_CODE_READ_MAIN_FILE 
		|| sub_operate == FIRMWARE_SUB_OPERATE_CODE_READ_USER_FILE || sub_operate == FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE){
		recv_bin_data data;
		data.set_err(nsp::proto::kSuccessful);
		memcpy_s(&data.offset_, sizeof(data.offset_), pkt->data_.c_str(), sizeof(data.offset_));
		memcpy_s(&data.data_length_, sizeof(data.data_length_), pkt->data_.substr(4, 4).c_str(), sizeof(data.data_length_));
		data.data_context_ = pkt->data_.substr(8, pkt->data_.size() - 8);
		nsp::toolkit::singleton<net_manager>::instance()->exec(pkt_id, (char *)&data);
	}
	else{
		recv_vcu_data data;
		data.set_err(nsp::proto::kSuccessful);
		data.remote_ip_ = r_ep.ipv4();
		nsp::toolkit::singleton<net_manager>::instance()->exec(pkt_id, (char *)&data);
	}
	
	return;
}

int network_session::post_vcu_type_request(const nsp::tcpip::endpoint& ep, 
	const std::shared_ptr<motion::asio_block> &asio_ack){
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_GET_INFO, FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE, pkt_id);
	c_data.data_len_ = 0;
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]() ->int {
		return psend(c_data, ep);
	}, asio_ack);
}

int network_session::post_cpu_type_requst(const nsp::tcpip::endpoint& ep, const std::shared_ptr<motion::asio_block> &asio_ack)
{
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_GET_INFO, FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION,pkt_id);
	c_data.data_len_ = 0;
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]()->int{
		return psend(c_data,ep);
	},asio_ack);
}

int network_session::post_vcu_version_request(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int is_control,
	const std::shared_ptr<motion::asio_block> &asio_ack){
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_GET_INFO, pkt_id);
	if (is_control == 1){
		switch (type)
		{
		case FIRMWARE_SOFTWARE_TYPE::BOOT_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_BOOT_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::MAIN_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_MAIN_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::USER_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_USER_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::CAMERA_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_CAMERA_VERSION);
			break;
		default:
			break;
		}
		c_data.data_len_ = 0;
	}
	else if (is_control == 0){
		switch (type)
		{
		case FIRMWARE_SOFTWARE_TYPE::BOOT_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_UNCONTROL_GET_BOOT_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::MAIN_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_UNCONTROL_GET_MAIN_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::USER_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_UNCONTROL_GET_USER_VERSION);
			break;
		case FIRMWARE_SOFTWARE_TYPE::CAMERA_SOFTWARE:
			c_data.set_sub_operate(FIRMWARE_UNCONTROL_GET_CAMERA_VERSION);
			break;
		default:
			break;
		}
		c_data.data_len_ = 4;
		uint32_t size = nsp::toolkit::singleton<file_read_handler>::instance()->get_file_size();
		char length[4];
		memcpy_s(length, sizeof(size), &size, sizeof(size));
		c_data.data_.assign(length, sizeof(size));
	}
	
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]() ->int {
		return psend(c_data, ep);
	}, asio_ack);
}

int network_session::post_vcu_reset_request(const nsp::tcpip::endpoint& ep, const uint32_t file_size,
	const std::shared_ptr<motion::asio_block> &asio_ack){
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_MODULE_CMD, FIRMWARE_SUB_OPERATE_CODE_RESET_VCU, pkt_id);
	c_data.data_len_ = sizeof(file_size);
	char buffer[4];
	memcpy_s(buffer, sizeof(file_size), &file_size, sizeof(file_size));
	c_data.data_.assign(buffer, sizeof(buffer));
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]() ->int {
		return psend(c_data, ep);
	}, asio_ack);
}

int network_session::post_vcu_write_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep,
	const int block_offset, const std::string&file_block_data,
	const std::shared_ptr<motion::asio_block>& asio_ack){
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, pkt_id);
	switch (type)
	{
	case FIRMWARE_SOFTWARE_TYPE::BOOT_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_BOOT_VERSION);
		break;
	case FIRMWARE_SOFTWARE_TYPE::MAIN_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_MAIN_VERSION);
		break;
	case FIRMWARE_SOFTWARE_TYPE::USER_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_USER_VERSION);
		break;
	case FIRMWARE_SOFTWARE_TYPE::CAMERA_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_GET_CAMERA_VERSION);
		break;
	default:
		break;
	}
	//由于发送的data部分前8个字节固定为offset和length，固此处需要添加改8个字节作为总长度
	c_data.data_len_ = file_block_data.size() + FIRMWARE_RW_FILE_BLOCK_LENGTH;
	//添加offset数据，length数据
	char off_len[8];
	uint32_t cb = file_block_data.size();
	memcpy_s(off_len, sizeof(block_offset), &block_offset, sizeof(block_offset));
	memcpy_s(&off_len[sizeof(block_offset)], sizeof(cb), &cb, sizeof(cb));
	c_data.data_.assign(off_len, sizeof(off_len));
	c_data.data_.append(file_block_data.substr(0, file_block_data.size()));
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]() ->int {
		return psend(c_data, ep);
	}, asio_ack);
}

int network_session::post_vcu_read_bin_file(const FIRMWARE_SOFTWARE_TYPE type, const nsp::tcpip::endpoint& ep, const int block_offset,
	const int buffer_length, const std::shared_ptr<motion::asio_block>& asio_ack){
	int pkt_id = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data c_data(FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE, pkt_id);
	switch (type)
	{
	case FIRMWARE_SOFTWARE_TYPE::BOOT_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_READ_BOOT_FILE);
		break;
	case FIRMWARE_SOFTWARE_TYPE::MAIN_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_READ_MAIN_FILE);
		break;
	case FIRMWARE_SOFTWARE_TYPE::USER_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_READ_USER_FILE);
		break;
	case FIRMWARE_SOFTWARE_TYPE::CAMERA_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_READ_CAMERA_FILE);
		break;
	case FIRMWARE_SOFTWARE_TYPE::COMPARE_SOFTWARE:
		c_data.set_sub_operate(FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE);
		break;
	default:
		break;
	}
	//此处长度固定为8个字节
	c_data.data_len_ = FIRMWARE_RW_FILE_BLOCK_LENGTH;
	//添加offset数据，length数据
	char data_buffer[8];
	memcpy_s(data_buffer, sizeof(block_offset), &block_offset, sizeof(block_offset));
	memcpy_s(&data_buffer[sizeof(block_offset)], sizeof(buffer_length), &buffer_length, sizeof(buffer_length));
	c_data.data_.assign(data_buffer, sizeof(data_buffer));
	return nsp::toolkit::singleton<net_manager>::instance()->write(pkt_id, [&]() ->int {
		return psend(c_data, ep);
	}, asio_ack);
}

int network_session::psend(const nsp::proto::proto_interface& package, const nsp::tcpip::endpoint& ep){
	return obudp::sendto(package.length(), [&](void*buffer, int cb)->int{
		return (package.serialize((unsigned char*)buffer) < 0) ? -1 : 0;
	}, ep);
}