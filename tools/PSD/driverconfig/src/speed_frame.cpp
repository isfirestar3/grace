#include "speed_frame.h"
#include "task_manager.h"
#include "net_task.h"
#include "network_client_manager.h"
#include "proto_udp_typedef.h"
#include "common/common_impls.hpp"
#include <thread>

speed_frame::speed_frame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->width(), this->height());
}

speed_frame::~speed_frame()
{

}

void speed_frame::init_rw_labels()
{
	ui.kp0_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.kp0_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.kp1_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.kp1_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.kp2_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.kp2_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.ki0_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.ki0_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.ki1_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.ki1_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.ki2_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.ki2_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.vel_dir_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.vel_dir_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.acc_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.acc_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.dec_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.dec_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.halt_dec_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.halt_dec_w_label->setText(tr("W:") + QUESTION_MARK);
}

void speed_frame::read_speed_pid(task_type_t task_type, uint16_t sub_index, funcalltask_t callback)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void speed_frame::write_speed_pid(task_type_t task_type, uint16_t sub_index, double kp, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_speed_pid(task_type, sub_index, kp, callback);
}

void speed_frame::write_vel_dir(task_type_t task_type, uint16_t sub_index, int16_t vel_dir_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_dir(task_type, sub_index, vel_dir_, callback);
}

void speed_frame::write_vel_acc(task_type_t task_type, uint16_t sub_index, int32_t vel_acc_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_acc(task_type, sub_index, vel_acc_, callback);
}

void speed_frame::write_vel_dec(task_type_t task_type, uint16_t sub_index, int32_t vel_dec_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_vel_dec(task_type, sub_index, vel_dec_, callback);
}

void speed_frame::write_halt_deceleration(task_type_t task_type, uint16_t sub_index, int32_t halt_deceleration_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_halt_deceleration(task_type, sub_index, halt_deceleration_, callback);
}

void speed_frame::set_speed_pid(double &kp, task_type_t task_type, speed_data &speed_data_)
{
	uint32_t kp_data = 0;

	if (ktask_type_read_register_speed_kp == task_type)
	{
		kp_data = speed_data_.kp_;
	}
	else if (ktask_type_read_register_speed_kl == task_type)
	{
		kp_data = speed_data_.ki_;
	}
	else
	{
		Q_ASSERT(false);

		return;
	}

	kp = kp_data / static_cast<double>(std::pow(2, 24));
}

void speed_frame::on_refresh()
{
	init_rw_labels();

	read_speed_pid(ktask_type_read_register_speed_kp, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.kp0_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kp, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_kp_0, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.kp0_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_speed_kp, 1, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.kp1_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kp, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_kp_1, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.kp1_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_speed_kp, 2, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.kp2_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kp, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_kp_2, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.kp2_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_speed_kl, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.ki0_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kl, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_ki_0, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.ki0_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_speed_kl, 1, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.ki1_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kl, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_ki_1, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.ki1_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_speed_kl, 2, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.ki2_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		double kp = 0;
		speed_data speed_data_ = *(speed_data *)str_data.data();
		set_speed_pid(kp, ktask_type_read_register_speed_kl, speed_data_);
		QMetaObject::invokeMethod(ui.doubleSpinBox_ki_2, "setValue", Qt::QueuedConnection,
			Q_ARG(double, kp));
		QMetaObject::invokeMethod(ui.ki2_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_veldir, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.vel_dir_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		vel_dir vel_dir_ = *(vel_dir *)str_data.data();
		QMetaObject::invokeMethod(ui.vel_dir_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, vel_dir_.vel_dir_));
		QMetaObject::invokeMethod(ui.vel_dir_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_vel_acc, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.acc_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		vel_acc_t vel_acc_t_ = *(vel_acc_t *)str_data.data();
		double vel_acc_t__ = (double)vel_acc_t_.vel_acc_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_acc, "setValue", Qt::QueuedConnection,
			Q_ARG(double, vel_acc_t__));
		QMetaObject::invokeMethod(ui.acc_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_vel_dec, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.dec_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		vel_dec_t vel_dec_t_ = *(vel_dec_t *)str_data.data();
		double vel_dec_t__ = (double)vel_dec_t_.vel_dec_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_dec, "setValue", Qt::QueuedConnection,
			Q_ARG(double, vel_dec_t__));
		QMetaObject::invokeMethod(ui.dec_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_halt_deceleration, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.halt_dec_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		halt_deceleration_t halt_deceleration_ = *(halt_deceleration_t *)str_data.data();
		double halt_deceleration__ = (double)halt_deceleration_.halt_deceleration_ / std::pow(2, 24);
		QMetaObject::invokeMethod(ui.doubleSpinBox_halt_dec, "setValue", Qt::QueuedConnection,
			Q_ARG(double, halt_deceleration__));
		QMetaObject::invokeMethod(ui.halt_dec_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});
}

void speed_frame::showEvent(QShowEvent *)
{
	on_refresh();
}

void speed_frame::on_saveKp0Btn_clicked()
{
	double kp = ui.doubleSpinBox_kp_0->value();
	write_speed_pid(ktask_type_write_register_speed_kp, 0, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.kp0_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.kp0_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveKp1Btn_clicked()
{
	double kp = ui.doubleSpinBox_kp_1->value();
	write_speed_pid(ktask_type_write_register_speed_kp, 1, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.kp1_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.kp1_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveKp2Btn_clicked()
{
	double kp = ui.doubleSpinBox_kp_2->value();
	write_speed_pid(ktask_type_write_register_speed_kp, 2, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.kp2_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.kp2_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveKi0Btn_clicked()
{
	double kp = ui.doubleSpinBox_ki_0->value();
	write_speed_pid(ktask_type_write_register_speed_kl, 0, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.ki0_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.ki0_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveKi1Btn_clicked()
{
	double kp = ui.doubleSpinBox_ki_1->value();
	write_speed_pid(ktask_type_write_register_speed_kl, 1, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.ki1_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.ki1_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveKi2Btn_clicked()
{
	double kp = ui.doubleSpinBox_ki_2->value();
	write_speed_pid(ktask_type_write_register_speed_kl, 2, kp, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.ki2_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.ki2_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_velDir_saveBtn_clicked()
{
	int vel_dir_ = ui.vel_dir_spinBox->value();
	write_vel_dir(ktask_type_write_register_veldir, 0, vel_dir_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.vel_dir_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.vel_dir_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveAccBtn_clicked()
{
	int32_t acc = ui.doubleSpinBox_acc->value() * std::pow(2, 24);
	write_vel_acc(ktask_type_write_register_vel_acc, 0, acc, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.acc_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.acc_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveDecBtn_clicked()
{
	int32_t dec = ui.doubleSpinBox_dec->value() * std::pow(2, 24);
	write_vel_acc(ktask_type_write_register_vel_dec, 0, dec, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.dec_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.dec_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_saveHaltDecBtn_clicked()
{
	int32_t halt_deceleration = ui.doubleSpinBox_halt_dec->value() * std::pow(2, 24);
	write_halt_deceleration(ktask_type_write_register_halt_deceleration, 0, halt_deceleration, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.halt_dec_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.halt_dec_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void speed_frame::on_refreshBtn_clicked()
{
	on_refresh();
}