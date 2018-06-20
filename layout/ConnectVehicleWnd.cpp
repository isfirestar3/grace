#include "ConnectVehicleWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qapplication.h>

ConnectVehicleWnd::ConnectVehicleWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


ConnectVehicleWnd::~ConnectVehicleWnd()
{
}

void ConnectVehicleWnd::InitWnd()
{
	resize(300, 100);
	setWindowTitle(en ? "Connect" : QStringLiteral("连接车辆"));
}

void ConnectVehicleWnd::InitCtrl()
{
	m_combConnectMode = new QComboBox;
	if (m_combConnectMode)
	{
		m_combConnectMode->addItem(en ? "Single" : QStringLiteral("定向连车"));
		m_combConnectMode->addItem(en ? "Broadcast" : QStringLiteral("广播模式"));
		m_combConnectMode->setCurrentIndex(0);
	}
	m_combSelectVehicle = new QComboBox;
	if (m_combSelectVehicle)
	{
		m_combSelectVehicle->addItem(en ? "Choice" : QStringLiteral("请选择"), QVariant(0));
		for (auto vehicle : m_vecVehicleInfo)
		{
			m_combSelectVehicle->addItem(QString::fromStdString(vehicle.vehicle_ip),QVariant(vehicle.vehicle_id));
		}
	}
	m_btnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
	m_editConnectState = new QLineEdit;
	if (m_editConnectState)
	{
		m_editConnectState->setText(en ? "DisConnected" : QStringLiteral("未连接"));
		m_editConnectState->setEnabled(false);
	}
}

void ConnectVehicleWnd::InitForm()
{
	QGridLayout* pMainLayout = new QGridLayout;
	if (pMainLayout)
	{
		QLabel* pLabelNetState = new QLabel(en ? "Status:" : QStringLiteral("连接状态："));
		pMainLayout->addWidget(pLabelNetState, 0, 0, 1, 1);
		pMainLayout->addWidget(m_editConnectState, 0, 1, 1, 1);

		QLabel* pLabelMode = new QLabel(en ? "Mode:" : QStringLiteral("连接模式："));
		pMainLayout->addWidget(pLabelMode, 1, 0, 1, 1);
		pMainLayout->addWidget(m_combConnectMode, 1, 1, 1, 1);
		QLabel* pLabelSelect = new QLabel(en ? "Select:" : QStringLiteral("选择车辆："));
		pMainLayout->addWidget(pLabelSelect, 1, 2, 1, 1);
		pMainLayout->addWidget(m_combSelectVehicle, 1, 3, 1, 1);
	}
	QVBoxLayout* pVLayout = new QVBoxLayout;
	if (pVLayout)
	{
		pVLayout->addLayout(pMainLayout);
		pVLayout->addWidget(m_btnConfirm);
	}
	setLayout(pVLayout);
}

void ConnectVehicleWnd::InitSlot()
{
	connect(m_btnConfirm, &QPushButton::clicked, this, &ConnectVehicleWnd::SlotBtnClickedConfirm);
}

void ConnectVehicleWnd::SlotBtnClickedConfirm()
{
	accept();
}

void ConnectVehicleWnd::SetVehicleInfo(std::vector<VEHICLEINFO>& vecVehicle)
{
	m_vecVehicleInfo = vecVehicle;
	ShowAllVehicle();
}

void ConnectVehicleWnd::ShowAllVehicle()
{
	if (m_combSelectVehicle)
	{
		for (auto vehicle : m_vecVehicleInfo)
		{
			m_combSelectVehicle->addItem(QString::fromStdString(vehicle.vehicle_ip), QVariant(vehicle.vehicle_id));
		}
	}
}

bool ConnectVehicleWnd::GetSelectVehicle(VEHICLEINFO& vehicleInfo)
{
	if (m_combSelectVehicle)
	{
		int index = m_combSelectVehicle->currentIndex();
		QVariant val = m_combSelectVehicle->itemData(index);
		int vehicle_id = val.toString().toInt();
		for (auto vehicle : m_vecVehicleInfo)
		{
			if (vehicle.vehicle_id == vehicle_id)
			{
				vehicleInfo = vehicle;
				return true;
			}
		}
	}
	return false;
}

void ConnectVehicleWnd::SetConnectVehicle(const VEHICLEINFO& vehicleInfo)
{
	if (vehicleInfo.vehicle_id <= 0)
	{
		return;
	}
	m_connectVehicle = vehicleInfo;
	if (m_combSelectVehicle)
	{
		for (int i = 1; i <= static_cast<int>(m_vecVehicleInfo.size()); ++i)
		{
			std::string text = m_combSelectVehicle->itemText(i).toLocal8Bit();
			if (text == vehicleInfo.vehicle_ip)
			{
				m_combSelectVehicle->setCurrentIndex(i);
			}
		}
	}
	if (m_editConnectState)
	{
		if (m_connectVehicle.vehicle_id > 0)
		{
			m_editConnectState->setText(en ? "Connected" : QStringLiteral("已连接"));
		}
		else
		{
			m_editConnectState->setText(en ? "DisConnected" : QStringLiteral("未连接"));
		}
	}
}