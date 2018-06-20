#include "position_frame.h"
#include "network_client_manager.h"
#include "common/common_impls.hpp"

position_frame::position_frame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->width(), this->height());
	ui.do_not_perform_rb->setChecked(true);
}

position_frame::~position_frame()
{

}

void position_frame::init_rw_labels()
{
	ui.homing_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.homing_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.pos_ki_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.pos_ki_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.pos_kp_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.pos_kp_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.jerk_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.jerk_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.acceleration_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.acceleration_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.velocity_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.velocity_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.kvff_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.kvff_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.kaff_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.kaff_w_label->setText(tr("W:") + QUESTION_MARK);
}

void position_frame::read_position_loop(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void position_frame::write_homing(task_type_t task_type, uint16_t sub_index, uint8_t homing_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_homing(task_type, sub_index, homing_, callback);
}

void position_frame::write_pos_ki(task_type_t task_type, uint16_t sub_index, int32_t pos_ki_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_pos_ki(task_type, sub_index, pos_ki_, callback);
}

void position_frame::write_pos_kp(task_type_t task_type, uint16_t sub_index, int32_t pos_kp_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_pos_kp(task_type, sub_index, pos_kp_, callback);
}

void position_frame::write_jerk(task_type_t task_type, uint16_t sub_index, float jerk_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_jerk(task_type, sub_index, jerk_, callback);
}

void position_frame::write_acceleration(task_type_t task_type, uint16_t sub_index, float acceleration_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_acceleration(task_type, sub_index, acceleration_, callback);
}

void position_frame::write_velocity(task_type_t task_type, uint16_t sub_index, float velocity_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_velocity(task_type, sub_index, velocity_, callback);
}

void position_frame::write_kvff(task_type_t task_type, uint16_t sub_index, int32_t kvff_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_kvff(task_type, sub_index, kvff_, callback);
}

void position_frame::write_kaff(task_type_t task_type, uint16_t sub_index, int32_t kaff_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_kaff(task_type, sub_index, kaff_, callback);
}

void position_frame::on_refresh()
{
	init_rw_labels();

	read_position_loop(ktask_type_read_register_homing, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.homing_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		homing_t homing_ = *(homing_t *)str_data.data();
		uint8_t homing__ = homing_.homing_;

		if (0 == homing__)
		{
			QMetaObject::invokeMethod(ui.do_not_perform_rb, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else if (1 == homing__)
		{
			QMetaObject::invokeMethod(ui.homing_on_rb, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else
		{
			Q_ASSERT(false);

			return;
		}

		QMetaObject::invokeMethod(ui.homing_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_pos_ki, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.pos_ki_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		pos_ki_t pos_ki_ = *(pos_ki_t *)str_data.data();
		double pos_ki__ = (double)pos_ki_.pos_ki_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_pos_ki, "setValue", Qt::QueuedConnection,
			Q_ARG(double, pos_ki__));
		QMetaObject::invokeMethod(ui.pos_ki_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_pos_kp, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.pos_kp_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		pos_kp_t pos_kp_ = *(pos_kp_t *)str_data.data();
		double pos_kp__ = (double)pos_kp_.pos_kp_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_pos_kp, "setValue", Qt::QueuedConnection,
			Q_ARG(double, pos_kp__));
		QMetaObject::invokeMethod(ui.pos_kp_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_jerk, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.jerk_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		jerk_t jerk_ = *(jerk_t *)str_data.data();
		double pos_kp__ = jerk_.jerk_;
		QMetaObject::invokeMethod(ui.doubleSpinBox_jerk, "setValue", Qt::QueuedConnection,
			Q_ARG(double, pos_kp__));
		QMetaObject::invokeMethod(ui.jerk_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_acceleration, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.acceleration_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		acceleration_t acc_t = *(acceleration_t *)str_data.data();
		QMetaObject::invokeMethod(ui.doubleSpinBox_acceleration, "setValue", Qt::QueuedConnection,
			Q_ARG(double, acc_t.acceleration_));
		QMetaObject::invokeMethod(ui.acceleration_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_velocity, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.velocity_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		velocity_t velocity_ = *(velocity_t *)str_data.data();
		QMetaObject::invokeMethod(ui.doubleSpinBox_velocity, "setValue", Qt::QueuedConnection,
			Q_ARG(double, velocity_.velocity_));
		QMetaObject::invokeMethod(ui.velocity_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_kvff, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.kvff_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		kvff_t kvff_ = *(kvff_t *)str_data.data();
		double kvff__ = (double)kvff_.kvff_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_kvff, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kvff__));
		QMetaObject::invokeMethod(ui.kvff_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_position_loop(ktask_type_read_register_kaff, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.kaff_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		kaff_t kaff_ = *(kaff_t *)str_data.data();
		double kaff__ = (double)kaff_.kaff_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_kaff, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kaff__));
		QMetaObject::invokeMethod(ui.kaff_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});
}

void position_frame::showEvent(QShowEvent *)
{
	on_refresh();
}

void position_frame::on_saveHomingBtn_clicked()
{
	uint8_t homing_;
	
	if (ui.homing_on_rb->isChecked())
	{
		homing_ = 1;
	}
	else if (ui.do_not_perform_rb->isChecked())
	{
		homing_ = 0;
	}
	else
	{
		Q_ASSERT(false);

		return;
	}

	write_homing(ktask_type_write_register_homing, 0, homing_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.homing_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.homing_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_savePosKiBtn_clicked()
{
	int32_t pos_ki_ = ui.doubleSpinBox_pos_ki->value() * std::pow(2, 24);
	write_pos_ki(ktask_type_write_register_pos_ki, 0, pos_ki_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.pos_ki_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.pos_ki_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_savePosKpBtn_clicked()
{
	int32_t pos_kp_ = ui.doubleSpinBox_pos_kp->value() * std::pow(2, 24);
	write_pos_kp(ktask_type_write_register_pos_kp, 0, pos_kp_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.pos_kp_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.pos_kp_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_saveJerkBtn_clicked()
{
	double jerk_ = ui.doubleSpinBox_jerk->value();
	write_jerk(ktask_type_write_register_jerk, 0, jerk_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.jerk_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.jerk_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_saveAccelerationBtn_clicked()
{
	double acceleration_ = ui.doubleSpinBox_acceleration->value();
	write_acceleration(ktask_type_write_register_acceleration, 0, acceleration_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.acceleration_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.acceleration_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_saveVelocityBtn_clicked()
{
	double velocity_ = ui.doubleSpinBox_velocity->value();
	write_velocity(ktask_type_write_register_velocity, 0, velocity_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.velocity_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.velocity_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_saveKvffBtn_clicked()
{
	int32_t kvff_ = ui.doubleSpinBox_kvff->value() * std::pow(2, 24);
	write_kvff(ktask_type_write_register_kvff, 0, kvff_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.kvff_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.kvff_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_saveKaffBtn_clicked()
{
	int32_t kaff_ = ui.doubleSpinBox_kaff->value() * std::pow(2, 24);
	write_kaff(ktask_type_write_register_kaff, 0, kaff_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.kaff_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.kaff_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void position_frame::on_refreshBtn_clicked()
{
	on_refresh();
}