#include "controldialog.h"
#include "controldialog_p.h"
#include "network_client_manager.h"
#include "common_impls.hpp"

namespace
{
	static const int MIN_DIAGNOSIS_CURRENT = 1;
	static const int MAX_DIAGNOSIS_CURRENT = 6;
	bool controlDlgClosed = false;
}

ControlDialog::ControlDialog(QWidget *parent)
: QDialog(parent),
d_ptr{QSharedPointer<ControlDialogPrivate>::create(this)}
{
	ui.setupUi(this);
	init();
	controlDlgClosed = false;
}

ControlDialog::~ControlDialog()
{

}

void ControlDialog::init()
{
	Q_D(ControlDialog);

	if (nullptr == d)
	{
		return;
	}

	d->init();
	initUi();
}

void ControlDialog::initUi()
{
	ui.switch_off_rb->setChecked(true);
	initCmb();
}

void ControlDialog::initCmb()
{
	int startIndex = static_cast<int>(ModeType::Mode_Diagnosis);
	int endIndex = static_cast<int>(ModeType::Mode_Position);
	ui.mode_cmb->clear();

	for (int i = startIndex; i <= endIndex; ++i)
	{
		ui.mode_cmb->addItem(ModeTypeToString(static_cast<ModeType>(i)), i);
	}

	ui.diagnosis_current_cmb->clear();

	for (int j = MIN_DIAGNOSIS_CURRENT; j <= MAX_DIAGNOSIS_CURRENT; ++j)
	{
		ui.diagnosis_current_cmb->addItem(QString::number(j), j);
	}
}

void ControlDialog::init_rw_labels()
{
	ui.switch_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.switch_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.clr_fault__w_label->setText(tr("W:") + QUESTION_MARK);
	ui.mode_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.mode_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.diagnosis_current_r_label->setText(tr("R:") + QUESTION_MARK);
	ui.diagnosis_current_w_label->setText(tr("W:") + QUESTION_MARK);
	ui.reset_caliburation_w_label->setText(tr("W:") + QUESTION_MARK);
}

void ControlDialog::read_speed_pid(task_type_t task_type, uint16_t sub_index, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->read_register_data(task_type, sub_index, callback);
}

void ControlDialog::write_switch(task_type_t task_type, uint16_t sub_index, uint8_t switch_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_switch(task_type, sub_index, switch_, callback);
}

void ControlDialog::write_clr_fault(task_type_t task_type, uint16_t sub_index, int32_t clr_fault_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_clr_fault(task_type, sub_index, clr_fault_, callback);
}

void ControlDialog::write_mode(task_type_t task_type, uint16_t sub_index, uint8_t mode_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_mode(task_type, sub_index, mode_, callback);
}

void ControlDialog::write_diagnosis_current(task_type_t task_type, uint16_t sub_index, uint32_t diagnosis_current_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_diagnosis_current(task_type, sub_index, diagnosis_current_, callback);
}

void ControlDialog::write_reset_caliburation(task_type_t task_type, uint16_t sub_index, uint8_t reset_caliburation_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_reset_caliburation(task_type, sub_index, reset_caliburation_, callback);
}

void ControlDialog::on_refresh()
{
	init_rw_labels();

	read_speed_pid(ktask_type_read_register_switch, 0, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.switch_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		switch_t switch_t_ = *(switch_t *)str_data.data();
		uint8_t switch_t__ = switch_t_.switch_;

		if (0 == switch_t__)
		{
			QMetaObject::invokeMethod(ui.switch_off_rb, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else if (1 == switch_t__)
		{
			QMetaObject::invokeMethod(ui.switch_on_rb, "setChecked", Qt::QueuedConnection,
				Q_ARG(bool, true));
		}
		else
		{
			Q_ASSERT(false);

			return;
		}

		QMetaObject::invokeMethod(ui.switch_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_mode, 1, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.mode_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		mode_t mode_t_ = *(mode_t *)str_data.data();
		uint8_t mode_ = mode_t_.mode_;
		int index = ui.mode_cmb->findData(mode_);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.mode_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.mode_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});

	std::this_thread::sleep_for(std::chrono::milliseconds(SEDN_INTERVAL));

	read_speed_pid(ktask_type_read_register_diagnosis_current, 1, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code || str_data.empty())
		{
			QMetaObject::invokeMethod(ui.diagnosis_current_r_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("R:") + CHA_HAO_MARK));

			return;
		}

		diagnosis_current_t diagnosis_current_ = *(diagnosis_current_t *)str_data.data();
		uint32_t diagnosis_current__ = diagnosis_current_.diagnosis_current_ / static_cast<double>(std::pow(2, 24));
		int index = ui.diagnosis_current_cmb->findData(diagnosis_current__);

		if (-1 != index)
		{
			QMetaObject::invokeMethod(ui.diagnosis_current_cmb, "setCurrentIndex", Qt::QueuedConnection,
				Q_ARG(int, index));
		}

		QMetaObject::invokeMethod(ui.diagnosis_current_r_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("R:") + DUI_HAO_MARK));
	});
}

QString ControlDialog::ModeTypeToString(ModeType modeType)
{
	QString mode;

	switch (modeType)
	{
	case ControlDialog::ModeType::Mode_Diagnosis:
	{
		mode = tr("Diagnosis");
	}
		break;
	case ControlDialog::ModeType::Mode_Current:
	{
		mode = tr("Current");
	}
		break;
	case ControlDialog::ModeType::Mode_Velocity:
	{
		mode = tr("Velocity");
	}
		break;
	case ControlDialog::ModeType::Mode_Position:
	{
		mode = tr("Position");
	}
		break;
	default:
	{
		Q_ASSERT(false);
	}
		break;
	}

	return mode;
}

void ControlDialog::showEvent(QShowEvent *)
{
	on_refresh();
}

void ControlDialog::closeEvent(QCloseEvent *)
{
	controlDlgClosed = true;
}

void ControlDialog::on_switch_save_btn_clicked()
{
	uint8_t switch_;

	if (ui.switch_on_rb->isChecked())
	{
		switch_ = 1;
	}
	else if (ui.switch_off_rb->isChecked())
	{
		switch_ = 0;
	}
	else
	{
		Q_ASSERT(false);

		return;
	}

	write_switch(ktask_type_write_register_switch, 0, switch_, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.switch_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.switch_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void ControlDialog::on_clr_fault_save_btn_clicked()
{
	uint8_t clr_fault_ = 1;
	write_clr_fault(ktask_type_write_register_clr_fault, 0, clr_fault_, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.clr_fault__w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.clr_fault__w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void ControlDialog::on_mode_save_btn_clicked()
{
	bool ok = false;
	uint8_t mode_ = ui.mode_cmb->itemData(ui.mode_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	write_mode(ktask_type_write_register_mode, 0, mode_, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.mode_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.mode_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void ControlDialog::on_diagnosis_current_save_btn_clicked()
{
	bool ok = false;
	int32_t diagnosis_current_ = ui.diagnosis_current_cmb->itemData(ui.diagnosis_current_cmb->currentIndex()).toInt(&ok);

	if (!ok)
	{
		return;
	}

	diagnosis_current_ *= std::pow(2, 24);
	write_clr_fault(ktask_type_write_register_diagnosis_current, 0, diagnosis_current_, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.diagnosis_current_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.diagnosis_current_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void ControlDialog::on_reset_caliburation_save_btn_clicked()
{
	uint8_t reset_caliburation_ = 1;
	write_clr_fault(ktask_type_write_register_reset_caliburation, 0, reset_caliburation_, [&](const std::string &str_data, int err_code)->void{
		if (controlDlgClosed)
		{
			return;
		}

		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(ui.reset_caliburation_w_label, "setText", Qt::QueuedConnection,
				Q_ARG(const QString &, tr("W:") + CHA_HAO_MARK));

			return;
		}

		QMetaObject::invokeMethod(ui.reset_caliburation_w_label, "setText", Qt::QueuedConnection,
			Q_ARG(const QString &, tr("W:") + DUI_HAO_MARK));
	});
}

void ControlDialog::on_refresh_btn_clicked()
{
	on_refresh();
}
