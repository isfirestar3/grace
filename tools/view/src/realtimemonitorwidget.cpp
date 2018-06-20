#include "realtimemonitorwidget.h"
#include "dbgvardialog.h"
#include "dbg_vars_impls.h"
#include "common/MessageBox.h"
#include <QtCore/QTimer>

namespace
{
	static const int TOOL_BTN_WIDTH = 32;
	static const int TOOL_BTN_HEIGHT = 32;
}

RealtimeMonitorWidget::RealtimeMonitorWidget(QWidget *parent)
: QWidget(parent),
m_rmModel{ QSharedPointer<RealtimeMonitorModel>::create() },
m_watchTimer{QSharedPointer<QTimer>::create()}
{
	ui.setupUi(this);
	InitUi();
	InitSlots();
}

RealtimeMonitorWidget::~RealtimeMonitorWidget()
{

}

void RealtimeMonitorWidget::InitUi()
{
	if (m_rmModel.isNull())
	{
		return;
	}

	QString name = IS_EN ? "Name" : "名称";
	QString data = IS_EN ? "Data" : "数据";
	m_rmModel->setHorizontalHeaderLabels(QStringList() << name << data);
	ui.treeView->setModel(m_rmModel.data());
	ui.treeView->setColumnWidth(RealtimeMonitorModel::RM_NAME, 200);

	ui.chooseBtn->SetPixmap(":/wid_operate/choose");
	ui.chooseBtn->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_WIDTH));
	ui.chooseBtn->setEnabled(false);
	ui.startBtn->SetPixmap(":/wid_operate/sys_start");
	ui.startBtn->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_WIDTH));
	ui.startBtn->setEnabled(false);
	ui.stopBtn->SetPixmap(":/wid_operate/sys_stop");
	ui.stopBtn->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_WIDTH));
	ui.stopBtn->setEnabled(false);

	QPalette palette;
	palette.setColor(QPalette::Background, QColor(221, 225, 230));
	setAutoFillBackground(true);
	setPalette(palette);
}

void RealtimeMonitorWidget::InitSlots()
{
	connect(m_watchTimer.data(), &QTimer::timeout, this, &RealtimeMonitorWidget::StartWatch);
	connect(ui.chooseBtn, &QPushButton::clicked, this, &RealtimeMonitorWidget::OnChooseDbgVar);
	connect(ui.startBtn, &QPushButton::clicked, this, &RealtimeMonitorWidget::OnStartWatch);
	connect(ui.stopBtn, &QPushButton::clicked, this, &RealtimeMonitorWidget::OnStopWatch);
}

void RealtimeMonitorWidget::UpdateNetStatus(int &netStatus, int &robotId)
{
	m_netStatus = netStatus;
	bool connected = false;

	if (mn::kNetworkStatus_Connected <= m_netStatus)
	{
		connected = true;
	}

	QMetaObject::invokeMethod(this, "UpdateBtnState", Qt::QueuedConnection, 
		QGenericReturnArgument(),
		Q_ARG(bool, connected));

	if (!m_rmModel.isNull())
	{
		m_rmModel->UpdateNetStatus(netStatus, robotId);
	}
}

void RealtimeMonitorWidget::OnChooseDbgVar()
{
	if (mn::kNetworkStatus_Connected > m_netStatus)
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "警告", IS_EN ? "Please connect the vehicle first!" : "请先连接车辆！", CMessageBox::Ok);

		return;
	}

	DbgVarDialog dvDialog(this);
	
	dvDialog.UpdateNetStatus(m_netStatus);
	dvDialog.SetChoosedMap(m_rmModel->GetChoosedMap());

	if (QDialog::Accepted == dvDialog.exec())
	{
		QMultiMap<QPair<int, uint32_t>, QString> choosedMap = dvDialog.GetChoosedMap();

		if (m_choosedMap == choosedMap)
		{
			return;
		}

		qMultiMapFree(m_choosedMap);
		m_choosedMap = choosedMap;

		if (!m_rmModel.isNull())
		{
			m_rmModel->SetChoosedMap(m_choosedMap);
			m_rmModel->InitModel();
		}
	}
}

void RealtimeMonitorWidget::OnStartWatch()
{
	if (mn::kNetworkStatus_Connected > m_netStatus)
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "警告", IS_EN ? "Please connect the vehicle first!" : "请先连接车辆！", CMessageBox::Ok);

		return;
	}

	if (m_watchTimer.isNull())
	{
		return;
	}

	int watchInterval = nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time();
	m_watchTimer->start(watchInterval);
	m_monitorStatus = RMMonitorStatus::Rm_Start;
	ui.startBtn->setEnabled(false);
	ui.stopBtn->setEnabled(true);
}

void RealtimeMonitorWidget::OnStopWatch()
{
	if (mn::kNetworkStatus_Connected > m_netStatus)
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "警告", IS_EN ? "Please connect the vehicle first!" : "请先连接车辆！", CMessageBox::Ok);

		return;
	}

	StopWatch();
	m_monitorStatus = RMMonitorStatus::Rm_Stop;
	ui.startBtn->setEnabled(true);
	ui.stopBtn->setEnabled(false);
}

void RealtimeMonitorWidget::StartWatch()
{
	if (m_rmModel.isNull())
	{
		return;
	}

	m_rmModel->StartWatch();
}

void RealtimeMonitorWidget::StopWatch()
{
	if (m_watchTimer.isNull())
	{
		return;
	}

	m_watchTimer->stop();
}

Q_INVOKABLE void RealtimeMonitorWidget::UpdateBtnState(bool enabled)
{
	if (m_watchTimer.isNull())
	{
		return;
	}

	ui.chooseBtn->setEnabled(enabled);
	ui.startBtn->setEnabled(enabled && !m_watchTimer->isActive());
	ui.stopBtn->setEnabled(enabled && m_watchTimer->isActive());
}

void RealtimeMonitorWidget::hideEvent(QHideEvent *)
{
	if (RMMonitorStatus::Rm_Start == m_monitorStatus)
	{
		m_monitorStatus = RMMonitorStatus::Rm_NoStatus;

		if (m_watchTimer->isActive())
		{
			m_watchTimer->stop();
		}

		ui.startBtn->setEnabled(true);
		ui.stopBtn->setEnabled(false);
	}
}
