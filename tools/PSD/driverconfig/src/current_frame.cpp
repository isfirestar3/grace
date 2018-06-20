#include "current_frame.h"
#include "network_client_manager.h"
#include "proto_udp_typedef.h"
#include "common/common_impls.hpp"
#include <thread>

current_frame::current_frame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	this->setFixedSize(this->width(),this->height());
}

current_frame::~current_frame()
{

}

void current_frame::init_rw_labels()
{
	ui.max_current_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.max_current_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.con_current_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.con_current_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.peak_time_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.peak_time_w_label->setText(tr("W:") + QUESTION_MARK);
}

void current_frame::on_refresh()
{
	init_rw_labels();

	read_current(ktask_type_read_register_max_current, 0, [&](const std::string &str_data, int err_code)->void{
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

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_current(ktask_type_read_register_con_current, 0, [&](const std::string &str_data, int err_code)->void{
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
}


void current_frame::read_current(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void current_frame::write_max_current(task_type_t task_type, uint16_t sub_index, uint8_t max_current_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_max_current(task_type, sub_index, max_current_, callback);
}

void current_frame::write_con_current(task_type_t task_type, uint16_t sub_index, uint8_t con_current_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_con_current(task_type, sub_index, con_current_, callback);
}

void current_frame::on_max_current_saveBtn_clicked()
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

void current_frame::on_con_current_saveBtn_clicked()
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

void current_frame::on_peak_time_saveBtn_clicked()
{

}

void current_frame::on_refreshBtn_clicked()
{
	on_refresh();
}

void current_frame::showEvent(QShowEvent *)
{
	on_refresh();
}
