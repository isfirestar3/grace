#include "di_frame.h"
#include "network_client_manager.h"
#include "common/common_impls.hpp"

di_frame::di_frame(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	initUi();
}

di_frame::~di_frame()
{

}

void di_frame::initUi()
{
	initCmb();
}

void di_frame::initCmb()
{
	int startIndex = static_cast<int>(DIConfigType::CT_LowEnable);
	int endIndex = static_cast<int>(DIConfigType::CT_Halt_High);
	initCmb(startIndex, endIndex, ui.di0_cmb);
	initCmb(startIndex, endIndex, ui.di1_cmb);
	initCmb(startIndex, endIndex, ui.di2_cmb);
	initCmb(startIndex, endIndex, ui.di3_cmb);
	initCmb(startIndex, endIndex, ui.di4_cmb);
	initCmb(startIndex, endIndex, ui.di5_cmb);
}

void di_frame::initCmb(int startIndex, int endIndex, QComboBox *pCmb)
{
	if (nullptr == pCmb)
	{
		return;
	}

	pCmb->clear();

	for (int i = startIndex; i <= endIndex; ++i)
	{
		pCmb->addItem(DIConfigTypeToString(static_cast<DIConfigType>(i)), i);
	}
}

void di_frame::init_rw_labels()
{
	ui.di0_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di0_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di1_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di1_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di2_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di2_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di3_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di3_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di4_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di4_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di5_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di5_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time0_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time0_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time1_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time1_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time2_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time2_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time3_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time3_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time4_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time4_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_filter_time5_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.di_filter_time5_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.di_state_r_label->setText(tr("R:") + QUESTION_MARK);
}

void di_frame::read_di_config(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void di_frame::write_di_config(task_type_t task_type, uint16_t sub_index, uint8_t di_config_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_config(task_type, sub_index, di_config_, callback);
}

void di_frame::write_di_filter_time(task_type_t task_type, uint16_t sub_index, uint16_t di_filter_time_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_filter_time(task_type, sub_index, di_filter_time_, callback);
}

void di_frame::write_di_state(task_type_t task_type, uint16_t sub_index, uint16_t di_state_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_di_state(task_type, sub_index, di_state_, callback);
}

void di_frame::write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_save(task_type, sub_index, save_, callback);
}

void di_frame::on_refresh()
{
	init_rw_labels();

	read_di_config(ktask_type_read_register_di_config, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di0_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di0_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di0_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di0_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_config, 1, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di1_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di1_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di1_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di1_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_config, 2, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di2_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di2_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di2_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di2_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_config, 3, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di3_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di3_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di3_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di3_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_config, 4, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di4_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di4_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di4_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di4_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_config, 5, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di5_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_config_t di_config_ = *(di_config_t *)str_data.data();
		uint8_t di_config_type = di_config_.di_config_;
		int index = ui.di5_cmb->findData(di_config_type);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.di5_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.di5_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time0_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time0_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time0_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 1, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time1_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time1_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time1_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 2, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time2_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time2_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time2_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 3, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time3_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time3_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time3_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 4, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time4_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time4_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time4_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_di_config(ktask_type_read_register_di_filter_time, 5, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_filter_time5_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_filter_time_t di_filter_time_ = *(di_filter_time_t *)str_data.data();
		uint16_t di_filter_time__ = di_filter_time_.di_filter_time_;

		QMetaObject::invokeMethod(ui.di_filter_time5_spinBox, "setValue", Qt::QueuedConnection,
			Q_ARG(int, di_filter_time__));
		QMetaObject::invokeMethod(ui.di_filter_time5_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	ui.di_state_label->clear();
	read_di_config(ktask_type_read_register_di_state, 0, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.di_state_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		di_state_t di_state_t_ = *(di_state_t *)str_data.data();
		uint16_t di_state__ = di_state_t_.di_state_;
		const std::string &ret_string = shr::gui::uint32_to_binary(di_state__);

		QMetaObject::invokeMethod(ui.di_state_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, QString::fromStdString(ret_string)));
		QMetaObject::invokeMethod(ui.di_state_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));
}

QString di_frame::DIConfigTypeToString(DIConfigType ct)
{
	QString ctString;

	switch (ct)
	{
	case di_frame::CT_LowEnable:
	{
		ctString = tr("LowEnable");
	}
		break;
	case di_frame::CT_LowEnable_with_Reset:
	{
		ctString = tr("LowEnable_with_Reset");
	}
		break;
	case di_frame::CT_HighEnable:
	{
		ctString = tr("HighEnable");
	}
		break;
	case di_frame::CT_HighEnable_with_Reset:
	{
		ctString = tr("HighEnable_with_Reset");
	}
		break;
	case di_frame::CT_Not_Configured:
	{
		ctString = tr("Not_Configured");
	}
		break;
	case di_frame::CT_Pos_Limit_Switch:
	{
		ctString = tr("Pos_Limit_Switch");
	}
		break;
	case di_frame::CT_Neg_Limit_Switch:
	{
		ctString = tr("Neg_Limit_Switch");
	}
		break;
	case di_frame::CT_Halt_Low:
	{
		ctString = tr("Halt_Low");
	}
		break;
	case di_frame::CT_Halt_High:
	{
		ctString = tr("Halt_High");
	}
		break;
	default:
	{
		Q_ASSERT(false);
	}
		break;
	}

	return ctString;
}

void di_frame::on_di0_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di0_cmb->itemData(ui.di0_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 0, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di0_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di0_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di1_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di1_cmb->itemData(ui.di1_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 1, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di1_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di1_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di2_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di2_cmb->itemData(ui.di2_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 2, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di2_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di2_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di3_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di3_cmb->itemData(ui.di3_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 3, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di3_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di3_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di4_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di4_cmb->itemData(ui.di4_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 4, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di4_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di4_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di5_saveBtn_clicked()
{
	bool ok = false;
	int di_config_ = ui.di5_cmb->itemData(ui.di5_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_di_config(ktask_type_write_register_di_config, 5, di_config_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di5_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di5_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time0_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time0_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 0, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time0_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time0_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time1_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time1_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 1, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time1_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time1_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time2_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time2_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 2, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time2_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time2_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time3_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time3_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 3, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time3_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time3_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time4_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time4_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 4, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time4_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time4_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_di_filter_time5_saveBtn_clicked()
{
	uint16_t di_filter_time_ = ui.di_filter_time5_spinBox->value();
	write_di_filter_time(ktask_type_write_register_di_filter_time, 5, di_filter_time_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.di_filter_time5_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.di_filter_time5_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void di_frame::on_refreshBtn_clicked()
{
	on_refresh();
}

void di_frame::showEvent(QShowEvent *)
{
	on_refresh();
}