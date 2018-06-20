#include "network_session.h"
#include "log.h"

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
	case START_DATA_TRANSMIT_FUNC:
	case CLOSE_DATA_TRANSMIT_FUNC:
	case FIRMWARE_MAIN_OPERATE_DATA_FORWARD:
	//case FIRMWARE_MAIN_OPERATE_CODE_CAN_DATA_CHANGE:
	//case FIRMWARE_CAN_COMMOND_START_DATA_FORWARD:
		dispatch_recv_info(upkt.main_operate(), upkt.pkt_id(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
		break;
	//case FIRMWARE_MAIN_OPERATE_CODE_GET_INFO:
	//	dispatch_vcu_get_info(upkt.pkt_id(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
	//	break;
	//case FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE:
	//	dispatch_vcu_rw_firmware(upkt.pkt_id(), upkt.sub_operate(), upkt.shared_for<nsp::proto::udp::recv_data>(), r_ep);
	//	break;
	default:
		break;
	}
}

void network_session::dispatch_recv_info(const uint16_t main_operate, const uint16_t pkt_id, const std::shared_ptr<nsp::proto::udp::recv_data>& pkt, const nsp::tcpip::endpoint &r_ep)
{
	recv_data data;
	data.set_err(nsp::proto::kSuccessful);
	//data.remote_ip_ = r_ep.ipv4();
	data.data_length_ = pkt->data_len_;
	data.data_context_ = pkt->data_;

	if (FIRMWARE_MAIN_OPERATE_DATA_FORWARD == main_operate)
	{
		nsp::proto::udp::can_datapacket_t  can_data;
		int length = pkt->data_len_;
		can_data.build((const unsigned char *)pkt->data_.data(), length);

		if (can_data.can_cmd != FIRMWARE_CAN_CMD_READ_WAVE_DATA)
		{
			nsp::toolkit::singleton<net_manager>::instance()->exec_data_forward(can_data.can_cmd, (char *)&data);
		}
		else
		{
			nsp::toolkit::singleton<net_manager>::instance()->exec_message(can_data.can_cmd, (char *)&data);
		}
	}
	else
	{
		nsp::toolkit::singleton<net_manager>::instance()->exec(pkt_id, (char *)&data);
	}
}

int network_session::post_start_data_transmit_func(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, nsp::tcpip::endpoint ep)
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(START_DATA_TRANSMIT_FUNC, code_serial, pktid);
	static const int DATA_LEN = 0x0004;
	data.data_len_ = DATA_LEN;
	unsigned int timeout = 5000;
	char buf[DATA_LEN] = { 0 };
	memcpy(buf, &timeout, DATA_LEN);
	data.data_.assign(buf, DATA_LEN);

	return nsp::toolkit::singleton<net_manager>::instance()->write(pktid, [&]()->int{
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_close_data_transmit_func(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, nsp::tcpip::endpoint ep)
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(CLOSE_DATA_TRANSMIT_FUNC, code_serial, pktid);
	data.data_len_ = 0x0000;
	data.data_.clear();

	return nsp::toolkit::singleton<net_manager>::instance()->write(pktid, [&]()->int{
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_read_register_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t register_index)
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_READ_REGISTER, register_index, nodeid);

	if ( assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_READ_REGISTER_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack );
}

int network_session::post_write_register_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t register_index, const std::string &strdata)
{
	const int DATA_LEN = 8;
	int str_data_len = strdata.length();

	if (str_data_len > DATA_LEN)
	{
		nsperror << __FUNCTION__ << "The data length is more than " << DATA_LEN;

		return -1;
	}

	char buf[DATA_LEN] = {0};
	memcpy(buf, strdata.data(), str_data_len);
	std::string data_;
	data_.assign(buf, DATA_LEN);

	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_WRITE_REGISTER, register_index, nodeid);
	can_data.data_length = data_.length();
	can_data.data_ = data_;

	if (assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_WRITE_REGISTER_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_read_version_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep)
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_GET_SOFTWARE_VERSION, 0, nodeid);

	if (assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_GET_SOFTWARE_VERSION_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_initial_rom_update_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep, uint16_t blocknumber )
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_INITIAL_ROM_UPDATE, 0, nodeid);

	uint16_t block_offset = 1;
	can_data.data_.append((char *)&block_offset, sizeof(block_offset));
	can_data.data_.append((char *)&blocknumber, sizeof(blocknumber));
	can_data.data_length = 4;

	if (assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_INITIAL_ROM_UPDATE_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_download_datapacket_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep,
	std::string & datastr, uint16_t index )
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET, index, nodeid);
	can_data.data_length = datastr.size();
	can_data.data_.assign(datastr.c_str(), datastr.size());

	if (assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack);
}

int network_session::post_abort_update_cmd(const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, nsp::tcpip::endpoint ep)
{
	uint16_t pktid = nsp::toolkit::singleton<net_manager>::instance()->pkt_id();
	nsp::proto::udp::common_data data(FIRMWARE_MAIN_OPERATE_DATA_FORWARD, code_serial, pktid);
	nsp::proto::udp::can_datapacket_t can_data(FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET, 0, nodeid);

	if (assign_data(data, can_data) != 0)
	{
		return -1;
	}

	return nsp::toolkit::singleton<net_manager>::instance()->write_data_forward(FIRMWARE_CAN_CMD_DOWNLOAD_DATAPACKET_ACK, [&]()->int {
		return psend(data, ep);
	}, asio_ack);
}

int network_session::assign_data(nsp::proto::udp::common_data & data, nsp::proto::udp::can_datapacket_t &candata)
{
	data.data_len_ = candata.length();
	unsigned char *pbuffer = nullptr;
	try
	{
		pbuffer = new unsigned char[data.data_len_];
	}
	catch (...)
	{
		return -1;
	}
	candata.serialize(pbuffer);
	data.data_.assign((char *)pbuffer, data.data_len_);
	delete[] pbuffer;

	return  0;
}

int network_session::psend(const nsp::proto::proto_interface& package, const nsp::tcpip::endpoint& ep){
	return obudp::sendto(package.length(), [&](void*buffer, int cb)->int{
		return (package.serialize((unsigned char*)buffer) < 0) ? -1 : 0;
	}, ep);
}
