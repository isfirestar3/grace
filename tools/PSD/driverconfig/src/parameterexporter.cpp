#include "parameterexporter.h"
#include "network_client_manager.h"
#include "singleton.hpp"
#include "common_impls.hpp"

ParameterExporter::ParameterExporter(const QString &filename)
:m_file{ filename }
{

}

ParameterExporter::~ParameterExporter()
{

}

bool ParameterExporter::openExportFile()
{
	return m_file.open(QIODevice::WriteOnly);
}

bool ParameterExporter::exportParameter()
{
	nsp::os::waitable_handle waitHandle;
	bool ret = false;
	switch_t switch_t_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_switch, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);

		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		switch_t_ = *(switch_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&switch_t_, sizeof(switch_t_)))
	{
		return false;
	}

	mode_t mode_t_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_mode, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig2(&waitHandle);
		Q_UNUSED(waitSig2);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		mode_t_ = *(mode_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&mode_t_, sizeof(mode_t_)))
	{
		return false;
	}

	diagnosis_current_t diagnosis_current_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_diagnosis_current, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		diagnosis_current_ = *(diagnosis_current_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&diagnosis_current_, sizeof(diagnosis_current_)))
	{
		return false;
	}

	motor_r motor_r_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_motor_r, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		motor_r_ = *(motor_r *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&motor_r_, sizeof(motor_r_)))
	{
		return false;
	}

	motor_l motor_l_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_motor_l, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		Q_UNUSED(waitSig);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		motor_l_ = *(motor_l *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&motor_l_, sizeof(motor_l_)))
	{
		return false;
	}

	polepairs polepairs_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_polepairs, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		polepairs_ = *(polepairs *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&polepairs_, sizeof(polepairs_)))
	{
		return false;
	}

	encoderlines encoderlines_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_encoderlines, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		encoderlines_ = *(encoderlines *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&encoderlines_, sizeof(encoderlines_)))
	{
		return false;
	}

	break_t break_t_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_break, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		break_t_ = *(break_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&break_t_, sizeof(break_t_)))
	{
		return false;
	}

	con_current_t con_current;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_con_current, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		con_current = *(con_current_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&con_current, sizeof(con_current)))
	{
		return false;
	}

	max_current_t max_current;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_max_current, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		max_current = *(max_current_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&max_current, sizeof(max_current)))
	{
		return false;
	}

	homing_t homing_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_homing, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		homing_ = *(homing_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&homing_, sizeof(homing_)))
	{
		return false;
	}

	pos_ki_t pos_ki_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_pos_ki, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		pos_ki_ = *(pos_ki_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&pos_ki_, sizeof(pos_ki_)))
	{
		return false;
	}

	pos_kp_t pos_kp_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_pos_kp, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		pos_kp_ = *(pos_kp_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&pos_kp_, sizeof(pos_kp_)))
	{
		return false;
	}

	jerk_t jerk_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_jerk, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		jerk_ = *(jerk_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&jerk_, sizeof(jerk_)))
	{
		return false;
	}

	acceleration_t acc_t;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_acceleration, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		acc_t = *(acceleration_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&acc_t, sizeof(acc_t)))
	{
		return false;
	}

	velocity_t velocity_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_velocity, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		velocity_ = *(velocity_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&velocity_, sizeof(velocity_)))
	{
		return false;
	}

	kvff_t kvff_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_kvff, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		kvff_ = *(kvff_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&kvff_, sizeof(kvff_)))
	{
		return false;
	}

	kaff_t kaff_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_kaff, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		kaff_ = *(kaff_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&kaff_, sizeof(kaff_)))
	{
		return false;
	}

	speed_data speed_data_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kp, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kp, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kp, 2, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kl, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kl, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_speed_kl, 2, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		speed_data_ = *(speed_data *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&speed_data_, sizeof(speed_data_)))
	{
		return false;
	}

	vel_dir vel_dir_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_veldir, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		vel_dir_ = *(vel_dir *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&vel_dir_, sizeof(vel_dir_)))
	{
		return false;
	}

	vel_acc_t vel_acc_t_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_vel_acc, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		vel_acc_t_ = *(vel_acc_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&vel_acc_t_, sizeof(vel_acc_t_)))
	{
		return false;
	}

	vel_dec_t vel_dec_t_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_vel_dec, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		vel_dec_t_ = *(vel_dec_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&vel_dec_t_, sizeof(vel_dec_t_)))
	{
		return false;
	}

	halt_deceleration_t halt_deceleration_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_halt_deceleration, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		halt_deceleration_ = *(halt_deceleration_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&halt_deceleration_, sizeof(halt_deceleration_)))
	{
		return false;
	}

	di_config_t di_config_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 2, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 3, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 4, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_config, 5, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_config_ = *(di_config_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_config_, sizeof(di_config_)))
	{
		return false;
	}

	di_filter_time_t di_filter_time_;
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 0, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 1, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 2, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 3, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 4, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(ktask_type_read_register_di_filter_time, 5, [&](const std::string &str_data, int err_code)->void{
		shr::gui::waitable_sig waitSig(&waitHandle);
		
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			ret = false;

			return;
		}

		di_filter_time_ = *(di_filter_time_t *)str_data.data();
		ret = true;
	});

	waitHandle.wait(TIMEOUT);

	if (!ret)
	{
		return false;
	}

	if (!write((const char *)&di_filter_time_, sizeof(di_filter_time_)))
	{
		return false;
	}

	return true;
}

bool ParameterExporter::write(const char *data, qint64 size)
{
	if (nullptr == data)
	{
		return false;
	}

	if (size <= 0)
	{
		return true;
	}

	qint64 writenLen = -1;

	while (writenLen = m_file.write(data, size))
	{
		if (-1 == writenLen)
		{
			return false;
		}

		size -= writenLen;

		if (0 == size)
		{
			break;
		}
	}

	return true;
}
