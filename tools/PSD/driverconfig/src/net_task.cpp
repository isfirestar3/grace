#include "net_task.h"
#include "network_client_manager.h"
#include "common/common_impls.hpp"

//std::map<int, uint8_t> net_task::register_index_map_ =
//{ 
//	{ (int)ktask_type_read_register_di_config, REGISTER_INDEX_DI_CONFIG },
//	{ (int)ktask_type_read_register_diagnosis_current, REGISTER_INDEX_DIAGNOSIS_CURRENT },
//	{ (int)ktask_type_read_register_mode, REGISTER_INDEX_MODE },
//	{ (int)ktask_type_read_register_vel_acc, REGISTER_INDEX_VEL_ACC },
//	{ (int)ktask_type_read_register_veldir, REGISTER_INDEX_VELDIR },
//	{ (int)ktask_type_read_register_speed_kp, REGISTER_INDEX_SPEED_KP },
//	{ (int)ktask_type_read_register_speed_kl, REGISTER_INDEX_SPEED_KI },
//	{ (int)ktask_type_read_register_max_current, REGISTER_INDEX_MAX_CURRENT },
//	{ (int)ktask_type_read_register_con_current, REGISTER_INDEX_CON_CURRENT },
//	{ (int)ktask_type_read_register_pos_kp, REGISTER_INDEX_POS_KP },
//	{ (int)ktask_type_read_register_pos_ki, REGISTER_INDEX_POS_KI },
//	{ (int)ktask_type_read_register_homing, REGISTER_INDEX_HOMING },
//	{ (int)ktask_type_read_register_break, REGISTER_INDEX_BREAK },
//	{ (int)ktask_type_read_register_polepairs, REGISTER_INDEX_POLEPAIRS },
//	{ (int)ktask_type_read_register_encoderlines, REGISTER_INDEX_ENCODERLINES },
//	{ (int)ktask_type_read_register_motor_r, REGISTER_INDEX_MOTOR_R },
//	{ (int)ktask_type_read_register_motor_l, REGISTER_INDEX_MOTOR_L },
//	{ (int)ktask_type_read_register_switch, REGISTER_INDEX_SWITCH },
//	{ (int)ktask_type_read_register_save, REGISTER_INDEX_SAVE },
//};
//
//std::recursive_mutex net_task::register_map_lock_;

net_task::net_task(const std::string &strlocal, const std::string &strhost, uint16_t port, uint8_t node_id, uint8_t sub_code, task_type_t task_type_operation, uint16_t register_index, funcalltask_t callback)
:ip_local_(strlocal),
ip_addr_(strhost),
port_(port),
node_id_(node_id),
sub_code_(sub_code),
task_type_operation_(task_type_operation),
register_index_(register_index),
function_callback_(callback)
{

}

net_task::net_task(const std::string &strlocal, const std::string &strhost, uint16_t port, uint8_t node_id, uint8_t sub_code, task_type_t task_type_operation, uint16_t register_index, const std::string &data, funcalltask_t callback)
:ip_local_(strlocal),
ip_addr_(strhost),
port_(port),
node_id_(node_id),
sub_code_(sub_code),
task_type_operation_(task_type_operation),
register_index_(register_index),
function_callback_(callback),
data_(data)
{

}

net_task::~net_task()
{

}

void net_task::on_task()
{
	if (task_type_operation_ == ktask_type_default)
	{
		
	}
	else if (ktask_type_start_data_transmit_func == task_type_operation_)
	{
		for (int index = 0; index < MAX_RETURE_NUMBER; index++)
		{
			if (0 == start_data_transmit_func())
			{
				break;
			}
		}
	}
	else if (ktask_type_close_data_transmit_func == task_type_operation_)
	{
		for (int index = 0; index < MAX_RETURE_NUMBER; index++)
		{
			if (0 == close_data_transmit_func())
			{
				break;
			}
		}
	}
	else if (task_type_operation_ > ktask_type_start_read_register
		&& task_type_operation_ < ktask_type_end_read_register)
	{
		for (int index = 0; index < MAX_RETURE_NUMBER; index++)
		{
			if (0 == read_register())
			{
				break;
			}
		}
	}
	else if (task_type_operation_ > ktask_type_start_write_register
		&& task_type_operation_ < ktask_type_end_write_register)
	{
		for (int index = 0; index < MAX_RETURE_NUMBER; index++)
		{
			if (0 == write_register(data_))
			{
				break;
			}
		}
	}
}

int net_task::start_data_transmit_func()
{
	bool successed = false;
	nsp::os::waitable_handle wh;
	recv_data recv_data_;

	int ret = nsp::toolkit::singleton<network_client_manager>::instance()->start_data_transmit_func(ip_addr_,
		std::make_shared<motion::asio_block>([&](const void *data){
		shr::gui::waitable_sig ws{ &wh };
		UNUSED(ws);

		if (nullptr == data)
		{
			nsperror << __FUNCTION__ << "The data is null.";

			return;
		}

		motion::asio_data *asio_data = (motion::asio_data *)data;
		int asio_err = asio_data->get_err();
		nspinfo << __FUNCTION__ << "The asio err is:" << asio_err;

		if (asio_err < 0)
		{
			nsperror << __FUNCTION__ << "The asio data get err is less than 0.";

			return;
		}

		recv_data_ = *(recv_data*)data;
		successed = true;
	}), sub_code_);

	nspinfo << __FUNCTION__ << "start_data_transmit_func ret is:" << ret;

	if (function_callback_)
	{
		function_callback_(ip_addr_, 0);
	}

	if (RET_FAILED == ret)
	{
		nsperror << __FUNCTION__ << "Call start_data_transmit_func func failed.";

		return -1;
	}

	shr::gui::waitable_wait ww{ &wh };
	UNUSED(ww);

	if (!ww.wait())
	{
		nsperror << __FUNCTION__ << "waitable_wait wait failed!";

		return -1;
	}

	if (!successed)
	{
		nsperror << __FUNCTION__ << "start_data_transmit_func failed!";

		return -1;
	}

	return 0;
}

int net_task::read_register()
{
	bool successed = false;
	nsp::os::waitable_handle wh;
	recv_data recv_data_;

	int ret = nsp::toolkit::singleton<network_client_manager>::instance()->post_read_register_cmd(ip_addr_,
		std::make_shared<motion::asio_block>([&](const void *data){
		shr::gui::waitable_sig ws{ &wh };
		UNUSED(ws);

		if (nullptr == data)
		{
			nsperror << __FUNCTION__ << "The data is null.";

			return;
		}

		motion::asio_data *asio_data = (motion::asio_data *)data;
		int asio_err = asio_data->get_err();
		nspinfo << __FUNCTION__ << "The asio err is:" << asio_err;

		if (asio_err < 0)
		{
			nsperror << __FUNCTION__ << "The asio data get err is less than 0.";

			return;
		}

		recv_data_ = *(recv_data*)data;
		successed = true;
	}), sub_code_, node_id_, register_index_);

	nspinfo << __FUNCTION__ << "post_read_register_cmd ret is:" << ret;

	if (RET_FAILED == ret)
	{
		nsperror << __FUNCTION__ << "Call post_read_register_cmd func failed.";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	shr::gui::waitable_wait ww{ &wh };
	UNUSED(ww);

	if (!ww.wait())
	{
		nsperror << __FUNCTION__ << "waitable_wait wait failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	if (!successed)
	{
		nsperror << __FUNCTION__ << "read_register failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	nsp::proto::udp::can_datapacket_t  can_data_packet;
	int length = recv_data_.data_length_;
	can_data_packet.build((const unsigned char *)recv_data_.data_context_.data(), length);

	if (0 == can_data_packet.data_length || RET_FAILED == check_can_read_data_packet(can_data_packet, node_id_, register_index_))
	{
		nsperror << __FUNCTION__ << "Check can read data packet failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	if (function_callback_)
	{
		function_callback_(can_data_packet.data_, nsp::proto::errorno_t::kSuccessful);
	}

	return 0;
}

int net_task::write_register(const std::string &str_data)
{
	bool successed = false;
	nsp::os::waitable_handle wh;
	recv_data recv_data_;

	int ret = nsp::toolkit::singleton<network_client_manager>::instance()->post_write_register_cmd(ip_addr_,
		std::make_shared<motion::asio_block>([&](const void *data){
		shr::gui::waitable_sig ws{ &wh };
		UNUSED(ws);

		if (nullptr == data)
		{
			nsperror << __FUNCTION__ << "The data is null.";

			return;
		}

		motion::asio_data *asio_data = (motion::asio_data *)data;
		int asio_err = asio_data->get_err();
		nspinfo << __FUNCTION__ << "The asio err is:" << asio_err;

		if (asio_err < 0)
		{
			nsperror << __FUNCTION__ << "The asio data get err is less than 0.";

			return;
		}

		recv_data_ = *(recv_data*)data;
		successed = true;
	}), sub_code_, node_id_, register_index_, str_data);

	nspinfo << __FUNCTION__ << "post_write_register_cmd ret is:" << ret;

	if (RET_FAILED == ret)
	{
		nsperror << __FUNCTION__ << "Call post_write_register_cmd func failed.";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	shr::gui::waitable_wait ww{ &wh };
	UNUSED(ww);

	if (!ww.wait())
	{
		nsperror << __FUNCTION__ << "waitable_wait wait failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	if (!successed)
	{
		nsperror << __FUNCTION__ << "write_register failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	nsp::proto::udp::can_datapacket_t  can_data_packet;
	int length = recv_data_.data_length_;
	can_data_packet.build((const unsigned char *)recv_data_.data_context_.data(), length);

	if (0 == can_data_packet.data_length || RET_FAILED == check_can_write_data_packet(can_data_packet, node_id_, register_index_))
	{
		nsperror << __FUNCTION__ << "Check can write data packet failed!";

		if (function_callback_)
		{
			function_callback_(std::string(), nsp::proto::errorno_t::kUnsuccessful);
		}

		return -1;
	}

	if (function_callback_)
	{
		function_callback_(can_data_packet.data_, nsp::proto::errorno_t::kSuccessful);
	}

	return 0;
}

int net_task::close_data_transmit_func()
{
	bool successed = false;
	nsp::os::waitable_handle wh;
	recv_data recv_data_;

	int ret = nsp::toolkit::singleton<network_client_manager>::instance()->close_data_transmit_func(ip_addr_,
		std::make_shared<motion::asio_block>([&](const void *data){
		shr::gui::waitable_sig ws{ &wh };
		UNUSED(ws);

		if (nullptr == data)
		{
			nsperror << __FUNCTION__ << "The data is null.";

			return;
		}

		motion::asio_data *asio_data = (motion::asio_data *)data;
		int asio_err = asio_data->get_err();
		nspinfo << __FUNCTION__ << "The asio err is:" << asio_err;

		if (asio_err < 0)
		{
			nsperror << __FUNCTION__ << "The asio data get err is less than 0.";

			return;
		}

		recv_data_ = *(recv_data*)data;
		successed = true;
	}), sub_code_);

	nspinfo << __FUNCTION__ << "close_data_transmit_func ret is:" << ret;

	if (function_callback_)
	{
		function_callback_(ip_addr_, 0);
	}

	if (RET_FAILED == ret)
	{
		nsperror << __FUNCTION__ << "Call close_data_transmit_func func failed.";

		return -1;
	}

	shr::gui::waitable_wait ww{ &wh };
	UNUSED(ww);

	if (!ww.wait())
	{
		nsperror << __FUNCTION__ << "waitable_wait wait failed!";

		return -1;
	}

	if (!successed)
	{
		nsperror << __FUNCTION__ << "close_data_transmit_func failed!";

		return -1;
	}

	return 0;
}

int net_task::check_can_read_data_packet(const nsp::proto::udp::can_datapacket_t &can_data_packet, uint8_t node_id, uint16_t register_index)
{
	if (FIRMWARE_CAN_CMD_READ_REGISTER_ACK != can_data_packet.can_cmd)
	{
		nsperror << __FUNCTION__ << "FIRMWARE_CAN_CMD_READ_REGISTER_ACK != can_data_packet.can_cmd!The can_data_packet.can_cmd is:" << can_data_packet.can_cmd;

		return RET_FAILED;
	}

	unsigned char ret = (can_data_packet.can_node_id >> 7) & 0x0001;
	nsperror << __FUNCTION__ << "The ret is:" << ret;
	CAN_RET can_ret = static_cast<CAN_RET>(ret);

	if (CAN_RET_SUCCESSED != can_ret)
	{
		nsperror << __FUNCTION__ << "CAN_RET_SUCCESSED != can_ret!";

		return RET_FAILED;
	}

	uint8_t node_id_ = (can_data_packet.can_node_id & 0x000f);
	nsperror << __FUNCTION__ << "The node_id_ is:" << node_id_ << "node_id is:" << node_id;

	if (node_id_ != node_id)
	{
		nsperror << __FUNCTION__ << "node_id_ != node_id!";

		return RET_FAILED;
	}

	uint8_t main_index = (register_index & 0x00ff);
	nsperror << __FUNCTION__ << "The main_index is:" << main_index << "can_data_packet.can_serial_index_0 is:" << can_data_packet.can_serial_index_0;

	if (main_index != can_data_packet.can_serial_index_0)
	{
		nsperror << __FUNCTION__ << "main_index != can_data_packet.can_serial_index_0!";

		return RET_FAILED;
	}

	uint8_t sub_index = (register_index >> 8 & 0x00ff);
	nsperror << __FUNCTION__ << "The sub_index is:" << sub_index << "can_data_packet.can_serial_index_1 is:" << can_data_packet.can_serial_index_1;

	if (sub_index != can_data_packet.can_serial_index_1)
	{
		nsperror << __FUNCTION__ << "sub_index != can_data_packet.can_serial_index_1!";

		return RET_FAILED;
	}

	return RET_SUCCESSED;
}

int net_task::check_can_write_data_packet(const nsp::proto::udp::can_datapacket_t &can_data_packet, uint8_t node_id, uint16_t register_index)
{
	if (FIRMWARE_CAN_CMD_WRITE_REGISTER_ACK != can_data_packet.can_cmd)
	{
		nsperror << __FUNCTION__ << "FIRMWARE_CAN_CMD_WRITE_REGISTER_ACK != can_data_packet.can_cmd!The can_data_packet.can_cmd is:" << can_data_packet.can_cmd;

		return RET_FAILED;
	}

	unsigned char ret = (can_data_packet.can_node_id >> 7) & 0x0001;
	nsperror << __FUNCTION__ << "The ret is:" << ret;
	CAN_RET can_ret = static_cast<CAN_RET>(ret);

	if (CAN_RET_SUCCESSED != can_ret)
	{
		nsperror << __FUNCTION__ << "CAN_RET_SUCCESSED != can_ret!";

		return RET_FAILED;
	}

	uint8_t node_id_ = (can_data_packet.can_node_id & 0x000f);
	nsperror << __FUNCTION__ << "The node_id_ is:" << node_id_ << "node_id is:" << node_id;

	if (node_id_ != node_id)
	{
		nsperror << __FUNCTION__ << "node_id_ != node_id!";

		return RET_FAILED;
	}

	uint8_t main_index = (register_index & 0x00ff);
	nsperror << __FUNCTION__ << "The main_index is:" << main_index << "can_data_packet.can_serial_index_0 is:" << can_data_packet.can_serial_index_0;

	if (main_index != can_data_packet.can_serial_index_0)
	{
		nsperror << __FUNCTION__ << "main_index != can_data_packet.can_serial_index_0!";

		return RET_FAILED;
	}

	uint8_t sub_index = (register_index >> 8 & 0x00ff);
	nsperror << __FUNCTION__ << "The sub_index is:" << sub_index << "can_data_packet.can_serial_index_1 is:" << can_data_packet.can_serial_index_1;

	if (sub_index != can_data_packet.can_serial_index_1)
	{
		nsperror << __FUNCTION__ << "sub_index != can_data_packet.can_serial_index_1!";

		return RET_FAILED;
	}

	return RET_SUCCESSED;
}
