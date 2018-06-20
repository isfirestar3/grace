#include "network_client_manager.h"
#include "log.h"
#include "net_task.h"
#include "task_manager.h"

network_client_manager::network_client_manager()
:net_session_{std::make_shared<network_session>()}
{

}

network_client_manager::~network_client_manager(){

}

void network_client_manager::read_register_data(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	std::map<int, uint8_t>::const_iterator iter = register_index_map_.find(static_cast<int>(task_type));

	if (register_index_map_.end() != iter)
	{
		uint8_t main_index = iter->second;
		main_index &= 0x00ff;
		sub_index = (sub_index << 8 & 0xff00);
		uint16_t register_index = (main_index | sub_index);
		nsp::toolkit::singleton<task_manager>::instance()->push_task(task_type, callback, register_index);
	}
}

void network_client_manager::write_register_data(task_type_t task_type, uint16_t sub_index, const std::string &str_data, funcalltask_t callback /*= nullptr*/)
{
	uint16_t register_index = 0;
	std::map<int, uint8_t>::const_iterator iter = register_index_map_.find((int)task_type);

	if (register_index_map_.end() != iter)
	{
		uint8_t main_index = iter->second;
		main_index &= 0x00ff;
		sub_index = (sub_index << 8 & 0xff00);
		uint16_t register_index = (main_index | sub_index);
		nsp::toolkit::singleton<task_manager>::instance()->push_task_attach_data(task_type, register_index, str_data, callback);
	}
}

void network_client_manager::write_speed_pid(task_type_t task_type, uint16_t sub_index, double kp, funcalltask_t callback /*= nullptr*/)
{
	int kp_data = kp * std::pow(2, 24);
	speed_data speed_data_;

	if (ktask_type_write_register_speed_kp == task_type)
	{
		speed_data_.kp_ = kp_data;
	}
	else if (ktask_type_write_register_speed_kl == task_type)
	{
		speed_data_.ki_ = kp_data;
	}
	else
	{
		assert(false);

		return;
	}

	const int BUF_SIZE = sizeof(speed_data);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &speed_data_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_vel_acc(task_type_t task_type, uint16_t sub_index, int32_t vel_acc_, funcalltask_t callback /* = nullptr */)
{
	vel_acc_t vel_acc_t_;
	vel_acc_t_.vel_acc_ = vel_acc_;
	const int BUF_SIZE = sizeof(vel_acc_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &vel_acc_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_vel_dec(task_type_t task_type, uint16_t sub_index, int32_t vel_dec_, funcalltask_t callback /*= nullptr*/)
{
	vel_dec_t vel_dec_t_;
	vel_dec_t_.vel_dec_ = vel_dec_;
	const int BUF_SIZE = sizeof(vel_dec_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &vel_dec_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_halt_deceleration(task_type_t task_type, uint16_t sub_index, int32_t halt_deceleration_, funcalltask_t callback /* = nullptr */)
{
	halt_deceleration_t halt_deceleration_t_;
	halt_deceleration_t_.halt_deceleration_ = halt_deceleration_;
	const int BUF_SIZE = sizeof(halt_deceleration_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &halt_deceleration_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_motor_r(task_type_t task_type, uint16_t sub_index, float motor_r_, funcalltask_t callback /*= nullptr*/)
{
	motor_r motor_r_t;
	motor_r_t.motor_r_ = motor_r_;
	const int BUF_SIZE = sizeof(motor_r_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &motor_r_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_motor_l(task_type_t task_type, uint16_t sub_index, float motor_l_, funcalltask_t callback /*= nullptr*/)
{
	motor_l motor_l_t;
	motor_l_t.motor_l_ = motor_l_;
	const int BUF_SIZE = sizeof(motor_l_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &motor_l_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_polepairs(task_type_t task_type, uint16_t sub_index, uint8_t polepairs_, funcalltask_t callback /*= nullptr*/)
{
	polepairs polepairs_t;
	polepairs_t.polepairs_ = polepairs_;
	const int BUF_SIZE = sizeof(polepairs_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &polepairs_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_encoderlines(task_type_t task_type, uint16_t sub_index, uint16_t encoderlines_, funcalltask_t callback /*= nullptr*/)
{
	encoderlines encoderlines_t;
	encoderlines_t.encoderlines_ = encoderlines_;
	const int BUF_SIZE = sizeof(encoderlines_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &encoderlines_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_vel_dir(task_type_t task_type, uint16_t sub_index, int16_t vel_dir_, funcalltask_t callback /*= nullptr*/)
{
	vel_dir vel_dir_t;
	vel_dir_t.vel_dir_ = vel_dir_;
	const int BUF_SIZE = sizeof(vel_dir_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &vel_dir_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_break_t(task_type_t task_type, uint16_t sub_index, bool break_t_, funcalltask_t callback /*= nullptr*/)
{
	break_t break_t__;
	break_t__.break_t_ = break_t_;
	const int BUF_SIZE = sizeof(break_t__);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &break_t__, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback /*= nullptr*/)
{
	max_current_t max_current;
	max_current.max_current = max_current_;
	const int BUF_SIZE = sizeof(max_current);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &max_current, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback /*= nullptr*/)
{
	con_current_t con_current;
	con_current.con_current = con_current_;
	const int BUF_SIZE = sizeof(con_current);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &con_current, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_di_config(task_type_t task_type, uint16_t sub_index, uint8_t di_config_, funcalltask_t callback /*= nullptr*/)
{
	di_config_t di_config_t_;
	di_config_t_.di_config_ = di_config_;
	const int BUF_SIZE = sizeof(di_config_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &di_config_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_di_filter_time(task_type_t task_type, uint16_t sub_index, uint16_t di_filter_time_, funcalltask_t callback /*= nullptr*/)
{
	di_filter_time_t di_filter_time_t_;
	di_filter_time_t_.di_filter_time_ = di_filter_time_;
	const int BUF_SIZE = sizeof(di_filter_time_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &di_filter_time_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_di_state(task_type_t task_type, uint16_t sub_index, uint16_t di_state_, funcalltask_t callback /*= nullptr*/)
{
	di_state_t di_state_t_;
	di_state_t_.di_state_ = di_state_;
	const int BUF_SIZE = sizeof(di_state_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &di_state_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback /*= nullptr*/)
{
	save_t save_t_;
	save_t_.save_ = save_;
	const int BUF_SIZE = sizeof(save_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &save_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_switch(task_type_t task_type, uint16_t sub_index, uint8_t switch_, funcalltask_t callback /* = nullptr */)
{
	switch_t switch_t_;
	switch_t_.switch_ = switch_;
	const int BUF_SIZE = sizeof(switch_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &switch_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_clr_fault(task_type_t task_type, uint16_t sub_index, int32_t clr_fault_, funcalltask_t callback /* = nullptr */)
{
	clr_fault_t clr_fault_t_;
	clr_fault_t_.clr_fault_ = clr_fault_;
	const int BUF_SIZE = sizeof(clr_fault_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &clr_fault_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_mode(task_type_t task_type, uint16_t sub_index, uint8_t mode_, funcalltask_t callback /* = nullptr */)
{
	mode_t mode_t_;
	mode_t_.mode_ = mode_;
	const int BUF_SIZE = sizeof(mode_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &mode_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_diagnosis_current(task_type_t task_type, uint16_t sub_index, uint32_t diagnosis_current_, funcalltask_t callback /* = nullptr */)
{
	diagnosis_current_t diagnosis_current_t_;
	diagnosis_current_t_.diagnosis_current_ = diagnosis_current_;
	const int BUF_SIZE = sizeof(diagnosis_current_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &diagnosis_current_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_reset_caliburation(task_type_t task_type, uint16_t sub_index, uint8_t reset_caliburation_, funcalltask_t callback /* = nullptr */)
{
	reset_caliburation_t reset_caliburation_t_;
	reset_caliburation_t_.reset_caliburation_ = reset_caliburation_;
	const int BUF_SIZE = sizeof(reset_caliburation_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &reset_caliburation_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_homing(task_type_t task_type, uint16_t sub_index, uint8_t homing_, funcalltask_t callback /*= nullptr*/)
{
	homing_t homing_t_;
	homing_t_.homing_ = homing_;
	const int BUF_SIZE = sizeof(homing_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &homing_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_pos_ki(task_type_t task_type, uint16_t sub_index, int32_t pos_ki_, funcalltask_t callback /*= nullptr*/)
{
	pos_ki_t pos_ki_t_;
	pos_ki_t_.pos_ki_ = pos_ki_;
	const int BUF_SIZE = sizeof(pos_ki_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &pos_ki_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_pos_kp(task_type_t task_type, uint16_t sub_index, int32_t pos_kp_, funcalltask_t callback /*= nullptr*/)
{
	pos_kp_t pos_kp_t_;
	pos_kp_t_.pos_kp_ = pos_kp_;
	const int BUF_SIZE = sizeof(pos_kp_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &pos_kp_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_jerk(task_type_t task_type, uint16_t sub_index, float jerk_, funcalltask_t callback /*= nullptr*/)
{
	jerk_t jerk_t_;
	jerk_t_.jerk_ = jerk_;
	const int BUF_SIZE = sizeof(jerk_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &jerk_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_acceleration(task_type_t task_type, uint16_t sub_index, float acceleration_, funcalltask_t callback /*= nullptr*/)
{
	acceleration_t acc_t;
	acc_t.acceleration_ = acceleration_;
	const int BUF_SIZE = sizeof(acc_t);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &acc_t, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_velocity(task_type_t task_type, uint16_t sub_index, float velocity_, funcalltask_t callback /*= nullptr*/)
{
	velocity_t velocity_t_;
	velocity_t_.velocity_ = velocity_;
	const int BUF_SIZE = sizeof(velocity_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &velocity_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_kvff(task_type_t task_type, uint16_t sub_index, int32_t kvff_, funcalltask_t callback /*= nullptr*/)
{
	kvff_t kvff_t_;
	kvff_t_.kvff_ = kvff_;
	const int BUF_SIZE = sizeof(kvff_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &kvff_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

void network_client_manager::write_kaff(task_type_t task_type, uint16_t sub_index, int32_t kaff_, funcalltask_t callback /*= nullptr*/)
{
	kaff_t kaff_t_;
	kaff_t_.kaff_ = kaff_;
	const int BUF_SIZE = sizeof(kaff_t_);
	char buf[BUF_SIZE] = { 0 };
	memcpy(buf, &kaff_t_, BUF_SIZE);
	std::string data_;
	data_.assign(buf, BUF_SIZE);
	nsp::toolkit::singleton<network_client_manager>::instance()->write_register_data(task_type, sub_index, data_, callback);
}

int network_client_manager::init_network(const std::string& ip_local, const std::string& ip_por_str){
	if (nullptr == net_session_.get())
	{
		return -1;
	}

	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	nsp::tcpip::endpoint ep;

	if (nsp::tcpip::endpoint::build(ip_por_str, ep) < 0){
		loinfo("udp_net") << "failed to build endpoint,the input string is " << ip_por_str;
		return -1;
	}

	if ( map_endpoint_.find(ep.ipv4()) == map_endpoint_.end())
	{
		map_endpoint_.insert(std::make_pair(ep.ipv4(), ep));
	}

	//if (net_session_){
	//	//loinfo("udp_net") << "ip is " << ep.ipv4() << " session is already exists.";
	//	return 0;
	//}

	//try{
	//	net_session_ = std::make_shared<network_session>();
	//}
	//catch (...){
	//	return -1;
	//}
	nsp::tcpip::endpoint ep_local(ip_local.c_str(), ep.port());

	if (net_session_){
		if (net_session_->create(ep_local) < 0){
			return -1;
		}

		return 0;
	}

	return -1;
}

int network_client_manager::clean_session(){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	if (net_session_){
		net_session_->close();
	}

	//net_session_.swap(std::shared_ptr<network_session>(nullptr));
	map_endpoint_.clear();
	return 0;
}

int network_client_manager::start_data_transmit_func(const std::string &ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial)
{
	if (nullptr == net_session_){
		return -1;
	}

	nsp::tcpip::endpoint ep = search_endpoint(ip);

	return net_session_->post_start_data_transmit_func(asio_ack, code_serial, ep);
}

int network_client_manager::post_read_register_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid,uint16_t register_index )
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_read_register_cmd(asio_ack, code_serial, nodeid, ep, register_index);
}

int network_client_manager::post_write_register_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, uint16_t register_index, const std::string &str_data)
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_write_register_cmd(asio_ack, code_serial, nodeid, ep, register_index, str_data);
}

int network_client_manager::close_data_transmit_func(const std::string &ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial)
{
	if (nullptr == net_session_){
		return -1;
	}

	nsp::tcpip::endpoint ep = search_endpoint(ip);

	return net_session_->post_close_data_transmit_func(asio_ack, code_serial, ep);
}

int network_client_manager::initial_rom_update(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid )
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_initial_rom_update_cmd(asio_ack, code_serial, nodeid, ep,5);
}

int network_client_manager::post_read_version_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid )
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_read_version_cmd(asio_ack, code_serial, nodeid, ep);
}

int network_client_manager::post_abort_update_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid)
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_abort_update_cmd(asio_ack, code_serial, nodeid, ep);
}

int network_client_manager::post_download_datapacket_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid,
	std::string & datastr, uint16_t index)
{
	nsp::tcpip::endpoint ep = search_endpoint(ip);

	if (!net_session_){
		return -1;
	}

	return net_session_->post_download_datapacket_cmd(asio_ack, code_serial, nodeid, ep, datastr,index);
}

//int network_client_manager::post_read_register_cmd(const std::string ip, const std::shared_ptr<motion::asio_block>& asio_ack, uint8_t code_serial, uint8_t nodeid, uint16_t register_index)
//{
//	nsp::tcpip::endpoint ep = search_endpoint(ip);
//
//	if (!net_session_){
//		return -1;
//	}
//
//	return net_session_->post_read_register_cmd(asio_ack, code_serial, nodeid, ep, register_index);
//}

nsp::tcpip::endpoint network_client_manager::search_endpoint(const std::string ip){
	std::lock_guard<decltype(re_mutex_)> lock(re_mutex_);
	nsp::tcpip::endpoint ep;
	auto iter = map_endpoint_.find(ip);
	if (iter == map_endpoint_.end()){
		return ep;
	}
	ep = iter->second;
	return ep;
}