#include "parameterimporter.h"
#include "network_client_manager.h"
#include "common_impls.hpp"

ParameterImporter::ParameterImporter(const QString &filename)
:m_file{ filename }
{

}

ParameterImporter::~ParameterImporter()
{

}

bool ParameterImporter::openImportFile()
{
	return m_file.open(QIODevice::ReadOnly);
}

bool ParameterImporter::importParameter()
{
	nsp::os::waitable_handle waitHandle;
	bool ret = false;
	switch_t switch_t_;

	if (!read((char *)&switch_t_, sizeof(switch_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_switch(ktask_type_write_register_switch, 0, switch_t_.switch_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	mode_t mode_t_;

	if (!read((char *)&mode_t_, sizeof(mode_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_mode(ktask_type_write_register_mode, 0, mode_t_.mode_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	diagnosis_current_t diagnosis_current_;

	if (!read((char *)&diagnosis_current_, sizeof(diagnosis_current_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_diagnosis_current(ktask_type_write_register_diagnosis_current, 0, diagnosis_current_.diagnosis_current_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	motor_r motor_r_;

	if (!read((char *)&motor_r_, sizeof(motor_r_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_motor_r(ktask_type_write_register_motor_r, 0, motor_r_.motor_r_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	motor_l motor_l_;

	if (!read((char *)&motor_l_, sizeof(motor_l_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_motor_l(ktask_type_write_register_motor_l, 0, motor_l_.motor_l_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	polepairs polepairs_;

	if (!read((char *)&polepairs_, sizeof(polepairs_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_polepairs(ktask_type_write_register_polepairs, 0, polepairs_.polepairs_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	encoderlines encoderlines_;

	if (!read((char *)&encoderlines_, sizeof(encoderlines_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_encoderlines(ktask_type_write_register_encoderlines, 0, encoderlines_.encoderlines_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	break_t break_t_;

	if (!read((char *)&break_t_, sizeof(break_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_break_t(ktask_type_write_register_break, 0, break_t_.break_t_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	con_current_t con_current_t_;

	if (!read((char *)&con_current_t_, sizeof(con_current_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_con_current(ktask_type_write_register_con_current, 0, con_current_t_.con_current, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	max_current_t max_current_t_;

	if (!read((char *)&max_current_t_, sizeof(max_current_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_max_current(ktask_type_write_register_max_current, 0, max_current_t_.max_current, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	homing_t homing_t_;

	if (!read((char *)&homing_t_, sizeof(homing_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_homing(ktask_type_write_register_homing, 0, homing_t_.homing_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	pos_ki_t pos_ki_t_;

	if (!read((char *)&pos_ki_t_, sizeof(pos_ki_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_pos_ki(ktask_type_write_register_pos_ki, 0, pos_ki_t_.pos_ki_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	pos_kp_t pos_kp_t_;

	if (!read((char *)&pos_kp_t_, sizeof(pos_kp_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_pos_kp(ktask_type_write_register_pos_kp, 0, pos_kp_t_.pos_kp_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	jerk_t jerk_t_;

	if (!read((char *)&jerk_t_, sizeof(jerk_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_jerk(ktask_type_write_register_jerk, 0, jerk_t_.jerk_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	acceleration_t vel_acc_t_;

	if (!read((char *)&vel_acc_t_, sizeof(vel_acc_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_acceleration(ktask_type_write_register_acceleration, 0, vel_acc_t_.acceleration_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	velocity_t velocity_t_;

	if (!read((char *)&velocity_t_, sizeof(velocity_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_velocity(ktask_type_write_register_velocity, 0, velocity_t_.velocity_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	kvff_t kvff_t_;

	if (!read((char *)&kvff_t_, sizeof(kvff_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_kvff(ktask_type_write_register_kvff, 0, kvff_t_.kvff_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	kaff_t kaff_t_;

	if (!read((char *)&kaff_t_, sizeof(kaff_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_kaff(ktask_type_write_register_kaff, 0, kaff_t_.kaff_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	speed_data speed_data_;

	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kp, 0, speed_data_.kp_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}


	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kp, 1, speed_data_.kp_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}


	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kp, 2, speed_data_.kp_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kl, 0, speed_data_.ki_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kl, 1, speed_data_.ki_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(ktask_type_write_register_speed_kl, 2, speed_data_.ki_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	vel_dir vel_dir_;

	if (!read((char *)&vel_dir_, sizeof(vel_dir_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_dir(ktask_type_write_register_veldir, 0, vel_dir_.vel_dir_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	vel_acc_t vel_acc_;

	if (!read((char *)&vel_acc_, sizeof(vel_acc_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_acc(ktask_type_write_register_vel_acc, 0, vel_acc_.vel_acc_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	vel_dec_t vel_dec_t_;

	if (!read((char *)&vel_dec_t_, sizeof(vel_dec_t_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_dec(ktask_type_write_register_vel_dec, 0, vel_dec_t_.vel_dec_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	halt_deceleration_t halt_deceleration_;

	if (!read((char *)&halt_deceleration_, sizeof(halt_deceleration_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_halt_deceleration(ktask_type_write_register_halt_deceleration, 0, halt_deceleration_.halt_deceleration_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	di_config_t di_config_;

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 0, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 1, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 2, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 3, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 4, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(ktask_type_write_register_di_config, 5, di_config_.di_config_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	di_filter_time_t di_filter_time_;

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 0, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 1, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 2, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 3, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 4, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!read((char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(ktask_type_write_register_di_filter_time, 5, di_filter_time_.di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			ret = false;

			return;
		}

		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	return true;
}

bool ParameterImporter::read(char *data, qint64 dataLen)
{
	if (nullptr == data || dataLen <= 0)
	{
		return false;
	}

	qint64 readLen = -1;

	while (readLen = m_file.read(data, dataLen))
	{
		if (-1 == readLen)
		{
			return false;
		}

		dataLen -= readLen;

		if (0 == dataLen)
		{
			break;
		}
	}

	return true;
}