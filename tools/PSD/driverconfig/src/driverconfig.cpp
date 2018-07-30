#include "driverconfig.h"
#include "controldialog.h"
#include "log.h"
#include "task_manager.h"
#include "frame_manager.h"
#include "parameterimporter.h"
#include "parameterexporter.h"
#include "network_client_manager.h"
#include "watcherdialog.hpp"
#include "common_impls.hpp"
#include <QFileDialog>
#include <QMessageBox>

driverconfig::driverconfig(QWidget *parent)
	: QMainWindow(parent)
{
	ui.setupUi(this);
	if ( nsp::toolkit::singleton<frame_manager>::instance()->init_frame() == -1)
	{
		QMessageBox::warning(NULL, "error", QString::fromLocal8Bit("初始化子窗口失败"));
	}
	initframe();
	initslot();
	nsp::toolkit::singleton<task_manager>::instance()->init_thread();
}

driverconfig::~driverconfig()
{
	nsp::toolkit::singleton<task_manager>::release();
}

void driverconfig::write_save(task_type_t task_type, uint16_t sub_index, uint32_t save_, funcalltask_t callback /*= nullptr*/)
{
	nsp::toolkit::singleton<network_client_manager>::instance()->write_save(task_type, sub_index, save_, callback);
}

void driverconfig::set_btns_enabled(bool enabled)
{
	ui.ipAddrLineEdit->setEnabled(!enabled);
	ui.portLineEdit->setEnabled(!enabled);
	ui.nodeIDLineEdit->setEnabled(!enabled);
	ui.canIDCmb->setEnabled(!enabled);
	ui.con_combobox->setEnabled(!enabled);
	ui.groupbox_config->setEnabled(enabled);
	ui.tab_motor->setEnabled(enabled);
	ui.tab_rom->setEnabled(enabled);
	ui.tab_scope->setEnabled(enabled);
	ui.import_btn->setEnabled(enabled);
	ui.export_btn->setEnabled(enabled);
}

void driverconfig::move_to_center(QWidget *p_widget)
{
	if (nullptr == p_widget)
	{
		return;
	}

	QRect dcRect = rect();
	QPoint topPoint = mapToGlobal(QPoint(dcRect.x(), dcRect.y()));
	int x = topPoint.x() + (dcRect.width() - p_widget->width()) / 2;
	int y = topPoint.y() + (dcRect.height() - p_widget->height()) / 2;
	p_widget->move(x, y);
}

void driverconfig::initframe()
{
 	ui.groupbox_config->setEnabled(false);
	ui.con_btn->setProperty("status",0);
	ui.tab_motor->setEnabled(false);
	ui.tab_rom->setEnabled(false);
	ui.tab_scope->setEnabled(false);
	ui.con_combobox->setCurrentIndex(0);
	ui.import_btn->setEnabled(false);
	ui.export_btn->setEnabled(false);
	ui.nodeIDLineEdit->setFocus();
}

void driverconfig::initslot()
{
	connect(this, SIGNAL(need_update_frame(int,QString)),this,SLOT(update_frame(int,QString)));
}

void driverconfig::on_con_btn_clicked()
{
	const QString &ipAddr = ui.ipAddrLineEdit->text();

	if (ipAddr.isEmpty())
	{
		QMessageBox::critical(this, tr("critical"), tr("The ip addr can not be empty!"));
		ui.ipAddrLineEdit->setFocus();

		return;
	}

	const QString &port = ui.portLineEdit->text();

	if (port.isEmpty())
	{
		QMessageBox::critical(this, tr("critical"), tr("The port can not be empty!"));
		ui.portLineEdit->setFocus();

		return;
	}

	const QString &nodeID = ui.nodeIDLineEdit->text();

	if (nodeID.isEmpty())
	{
		QMessageBox::critical(this, tr("critical"), tr("The node ID can not be empty!"));
		ui.nodeIDLineEdit->setFocus();

		return;
	}

	std::string ipAddrLocal = std::string("0.0.0.0");
	std::string ipAddrRemote = ipAddr.toStdString();
	bool ok = false;
	uint16_t port_t = port.toUShort(&ok, 16);

	if (!ok)
	{
		return;
	}

	int conStatusProperty = ui.con_btn->property("status").toInt(&ok);

	if (!ok)
	{
		return;
	}

	if (0 == conStatusProperty)
	{
		if (0 != nsp::toolkit::singleton<network_client_manager>::instance()->init_network(ipAddrLocal, ipAddrRemote + ":" + QString::number(port_t).toStdString()))
		{
			QMessageBox::critical(this, tr("critical"), tr("Init the network failed!"));

			return;
		}
	}
	else if (1 == conStatusProperty)
	{
		if (0 != nsp::toolkit::singleton<network_client_manager>::instance()->clean_session())
		{
			QMessageBox::critical(this, tr("critical"), tr("Close the network failed!"));

			return;
		}
	}
	else
	{
		Q_ASSERT(false);
	}

	if (0 == conStatusProperty)
	{
		ui.con_btn->setProperty("status", 1);
		ui.con_btn->setText("Disconnect driver");
		set_btns_enabled(true);

		uint8_t node_id = static_cast<uint8_t>(nodeID.toInt(&ok));

		if (!ok)
		{
			return;
		}

		int conCmbIndex = ui.con_combobox->currentIndex();
		int canIDCmbIndex = ui.canIDCmb->currentIndex();

		if (-1 == conCmbIndex || -1 == canIDCmbIndex)
		{
			return;
		}

		QString subCode = QString("%1%2").arg(conCmbIndex).arg(canIDCmbIndex);
		uint8_t sub_code = static_cast<uint8_t>(subCode.toInt(&ok, 16));

		if (!ok)
		{
			return;
		}

		nsp::toolkit::singleton<task_manager>::instance()->init_net_work(ipAddrLocal, ipAddrRemote, port_t, node_id, sub_code);
		nsp::toolkit::singleton<task_manager>::instance()->push_task(ktask_type_start_data_transmit_func, [&](const std::string &str_data, int err_code)->void{
			if (nsp::proto::errorno_t::kSuccessful != err_code)
			{
				return;
			}
		});
	}
	else if (1 == conStatusProperty)
	{
		ui.con_btn->setProperty("status", 0);
		ui.con_btn->setText("Connect driver");
		set_btns_enabled(false);

		nsp::toolkit::singleton<task_manager>::instance()->push_task(ktask_type_close_data_transmit_func, [&](const std::string &str_data, int err_code)->void{
			if (nsp::proto::errorno_t::kSuccessful != err_code)
			{
				return;
			}
		});
	}
	else
	{
		Q_ASSERT(false);
	}
}

void driverconfig::on_import_btn_clicked()
{
	const QString &filename = QFileDialog::getOpenFileName(this, tr("Import parameter"), QDir::currentPath(), tr("*.bin"));

	if (filename.isEmpty())
	{
		return;
	}

	ParameterImporter importer(filename);
	bool(ParameterImporter::*func)() = &ParameterImporter::importParameter;
	const std::function<void(bool)> &retCb = [&](bool ret)->void{
		if (ret)
		{
			QMessageBox::information(this, tr("Information"), tr("Import successed"));
		}
		else
		{
			QMessageBox::critical(this, tr("Critical"), tr("Import failed"));
		}
	};

	nsp::toolkit::singleton<gdp::gui::WatcherDialog>::instance()->run(&importer, func, retCb, this);
}

void driverconfig::on_export_btn_clicked()
{
	const QString &filename = QFileDialog::getSaveFileName(this, tr("Export parameter"), QDir::currentPath(), tr("*.bin"));

	if (filename.isEmpty())
	{
		return;
	}

	ParameterExporter exporter(filename);
	bool(ParameterExporter::*func)() = &ParameterExporter::exportParameter;
	const std::function<void(bool)> &retCb = [&](bool ret)->void{
		if (ret)
		{
			QMessageBox::information(this, tr("Information"), tr("Export successed"));
		}
		else
		{
			QMessageBox::critical(this, tr("Critical"), tr("Export failed"));
		}
	};

	nsp::toolkit::singleton<gdp::gui::WatcherDialog>::instance()->run(&exporter, func, retCb, this);
}

void driverconfig::on_btn_control_clicked()
{
	ControlDialog controlDlg(this);
	controlDlg.exec();
}

void driverconfig::on_btn_motor_parameter_clicked()
{
	QWidget * pmotor_frame = nsp::toolkit::singleton<frame_manager>::instance()->getmotorframe();

	if (pmotor_frame != nullptr && pmotor_frame->isVisible() == false)
	{
		move_to_center(pmotor_frame);
		pmotor_frame->setWindowModality(Qt::ApplicationModal);
		pmotor_frame->show();
	}
}

void driverconfig::on_btn_motor_diagnosis_clicked()
{

}

void driverconfig::on_btn_driver_setting_clicked()
{

}

void driverconfig::on_btn_dio_clicked()
{
	QWidget * pdi_frame = nsp::toolkit::singleton<frame_manager>::instance()->getdiframe();

	if (pdi_frame != nullptr  && pdi_frame->isVisible() == false )
	{
		move_to_center(pdi_frame);
		pdi_frame->setWindowModality(Qt::ApplicationModal);
		pdi_frame->show();
	}
}

void driverconfig::on_btn_save_clicked()
{
	uint32_t save_ = 0x31415926;
	write_save(ktask_type_write_register_save, 0, save_, [&](const std::string &str_data, int err_code)->void{
		if (nsp::proto::errorno_t::kSuccessful != err_code)
		{
			QMetaObject::invokeMethod(this, "update_save_btn_state", Qt::QueuedConnection,
				Q_ARG(bool, false));

			return;
		}

		QMetaObject::invokeMethod(this, "update_save_btn_state", Qt::QueuedConnection,
			Q_ARG(bool, true));
	});
}

void driverconfig::on_btn_position_loop_clicked()
{
	QWidget * pposition_frame = nsp::toolkit::singleton<frame_manager>::instance()->getpositionframe();

	if (pposition_frame != nullptr && pposition_frame->isVisible() == false )
	{
		pposition_frame->setWindowModality(Qt::ApplicationModal);
		pposition_frame->show();
	}
}

void driverconfig::on_btn_speed_loop_clicked()
{
	QWidget * pspeed_frame = nsp::toolkit::singleton<frame_manager>::instance()->getspeedframe();

	if (pspeed_frame != nullptr && pspeed_frame->isVisible() == false )
	{
		move_to_center(pspeed_frame);
		pspeed_frame->setWindowModality(Qt::ApplicationModal);
		pspeed_frame->show();
	}
}

void driverconfig::on_btn_current_loop_clicked()
{
	QWidget * pcurrent_frame = nsp::toolkit::singleton<frame_manager>::instance()->getcurrentframe();

	if (pcurrent_frame != nullptr && pcurrent_frame->isVisible() == false)
	{
		move_to_center(pcurrent_frame);
		pcurrent_frame->setWindowModality(Qt::ApplicationModal);
		pcurrent_frame->show();
	}
}

void driverconfig::on_btn_upload_clicked()
{

}

void driverconfig::on_btn_download_clicked()
{

}

void driverconfig::update_frame(int tasktype, QString qstrdata)
{
	switch (tasktype)
	{
	default:
		break;
	}
}

void driverconfig::update_save_btn_state(bool ret)
{
	const QString &saveBtnText = tr("Save");

	if (ret)
	{
		ui.btn_save->setText(saveBtnText + LEFT_BRACKET + DUI_HAO_MARK + RIGHT_BRACKET);
	}
	else
	{
		ui.btn_save->setText(saveBtnText + LEFT_BRACKET + CHA_HAO_MARK + RIGHT_BRACKET);
	}
}
