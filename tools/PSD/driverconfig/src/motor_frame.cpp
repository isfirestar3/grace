#include "motor_frame.h"
#include "task_manager.h"
#include "driverconfig.h"
#include "network_client_manager.h"
#include "common/common_impls.hpp"

motor_frame::motor_frame(QWidget *parent)
	: QWidget(NULL)
{
	ui.setupUi(this);
	this->setFixedSize(this->width(), this->height());
}

motor_frame::~motor_frame()
{
	
}

void motor_frame::init_rw_labels()
{
	ui.motor_r_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.motor_r_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.motor_l_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.motor_l_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.polepairs_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.polepairs_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.encoderline_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.encoderline_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.break_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.break_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.con_current_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.con_current_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.max_current_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.max_current_w_label->setText(tr("W:") + QUESTION_MARK);
}

void motor_frame::read_motor_specified(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void motor_frame::write_motor_r(task_type_t task_type, uint16_t sub_index, float motor_r_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_motor_r(task_type, sub_index, motor_r_, callback);
}

void motor_frame::write_motor_l(task_type_t task_type, uint16_t sub_index, float motor_l_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_motor_l(task_type, sub_index, motor_l_, callback);
}

void motor_frame::write_polepairs(task_type_t task_type, uint16_t sub_index, uint8_t polepairs_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_polepairs(task_type, sub_index, polepairs_, callback);
}

void motor_frame::write_encoderlines(task_type_t task_type, uint16_t sub_index, uint16_t encoderlines_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_encoderlines(task_type, sub_index, encoderlines_, callback);
}

void motor_frame::write_break_t(task_type_t task_type, uint16_t sub_index, bool break_t_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_break_t(task_type, sub_index, break_t_, callback);
}

void motor_frame::write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_con_current(task_type, sub_index, con_current_, callback);
}

void motor_frame::write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_max_current(task_type, sub_index, max_current_, callback);
}

void motor_frame::on_refresh()
{
	init_rw_labels();

	read_motor_specified(ktask_type_read_register_motor_r, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.motor_r_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		motor_r motor_r_ = *(motor_r *)str_data.data();
		QMetaObject::invokeMethod(ui.motor_r_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(double, motor_r_.motor_r_));
		QMetaObject::invokeMethod(ui.motor_r_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_motor_l, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.motor_l_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		motor_l motor_l_ = *(motor_l *)str_data.data();
		QMetaObject::invokeMethod(ui.motor_l_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(double, motor_l_.motor_l_));
		QMetaObject::invokeMethod(ui.motor_l_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_polepairs, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.polepairs_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		polepairs polepairs_ = *(polepairs *)str_data.data();
		QMetaObject::invokeMethod(ui.polepairs_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, polepairs_.polepairs_));
		QMetaObject::invokeMethod(ui.polepairs_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_encoderlines, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.encoderline_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		encoderlines encoderlines_ = *(encoderlines *)str_data.data();
		QMetaObject::invokeMethod(ui.encoderline_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, encoderlines_.encoderlines_));
		QMetaObject::invokeMethod(ui.encoderline_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_break, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.break_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		break_t break_t_ = *(break_t *)str_data.data();

		if (break_t_.break_t_)
		{
			QMetaObject::invokeMethod(ui.break_enable, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else
		{
			QMetaObject::invokeMethod(ui.break_disable, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}

		QMetaObject::invokeMethod(ui.break_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_con_current, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.con_current_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		con_current_t con_current = *(con_current_t *)str_data.data();
		QMetaObject::invokeMethod(ui.spinBox_con_current, "setValue", Qt::QueuedConnection,
			Q_ARG(int, con_current.con_current));
		QMetaObject::invokeMethod(ui.con_current_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_motor_specified(ktask_type_read_register_max_current, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.max_current_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		max_current_t max_current = *(max_current_t *)str_data.data();
		QMetaObject::invokeMethod(ui.spinBox_max_current, "setValue", Qt::QueuedConnection,
			Q_ARG(int, max_current.max_current));
		QMetaObject::invokeMethod(ui.max_current_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_motor_r_saveBtn_clicked()
{
	double motor_t_ = ui.motor_r_spinBox->value();
	write_motor_r(ktask_type_write_register_motor_r, 0, motor_t_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.motor_r_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.motor_r_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_motor_l_saveBtn_clicked()
{
	double motor_l_ = ui.motor_l_spinBox->value();
	write_motor_l(ktask_type_write_register_motor_l, 0, motor_l_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.motor_l_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.motor_l_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_polepairs_saveBtn_clicked()
{
	int polepairs_ = ui.polepairs_spinBox->value();
	write_polepairs(ktask_type_write_register_polepairs, 0, polepairs_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.polepairs_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.polepairs_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_encoderline_saveBtn_clicked()
{
	int encoderlines_ = ui.encoderline_spinBox->value();
	write_encoderlines(ktask_type_write_register_encoderlines, 0, encoderlines_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.encoderline_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.encoderline_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_break_saveBtn_clicked()
{
	bool break_t_ = true;

	if (ui.break_disable->isChecked())
	{
		break_t_ = false;
	}

	write_break_t(ktask_type_write_register_break, 0, break_t_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.break_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.break_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_con_current_saveBtn_clicked()
{
	int con_current = ui.spinBox_con_current->value();
	write_con_current(ktask_type_write_register_con_current, 0, con_current, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.con_current_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.con_current_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_max_current_saveBtn_clicked()
{
	int max_current = ui.spinBox_max_current->value();
	write_max_current(ktask_type_write_register_max_current, 0, max_current, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.max_current_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.max_current_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void motor_frame::on_refreshBtn_clicked()
{
	on_refresh();
}

void motor_frame::showEvent(QShowEvent *)
{
	on_refresh();
}