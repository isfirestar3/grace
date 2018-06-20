#include "VehicleInfoWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include "point_algorithm.hpp"
#include <QtWidgets/qapplication.h>

VehicleInfoWnd::VehicleInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


VehicleInfoWnd::~VehicleInfoWnd()
{
}

void VehicleInfoWnd::InitWnd()
{
	resize(300, 200);
	setWindowTitle(en ? "VehicleInfo" : QStringLiteral("车量信息"));
}

void VehicleInfoWnd::InitCtrl()
{
	m_pEditAngle = new QLineEdit;
	m_pEditVehicleId = new QLineEdit;
	m_pEditPosx = new QLineEdit;
	m_pEditPosy = new QLineEdit;
	if (m_pEditVehicleId)
	{
		m_pEditVehicleId->setEnabled(false);
	}
	m_pBtnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
}

void VehicleInfoWnd::InitForm()
{
	QGridLayout* pGridLayout = new QGridLayout;
	if (pGridLayout)
	{
		QLabel* pLabelAngle = new QLabel(en ? "Angle:" : QStringLiteral("角度（度）："));
		QLabel* pLabelId = new QLabel(en ? "Id:" : QStringLiteral("ID："));
		QLabel* pLabelPosx = new QLabel(en ? "X:" : QStringLiteral("位置X："));
		QLabel* pLabelPosy = new QLabel(en ? "Y:" : QStringLiteral("位置Y："));
		pGridLayout->addWidget(pLabelId, 0, 0, 1, 1);
		pGridLayout->addWidget(m_pEditVehicleId, 0, 1, 1, 1);
		pGridLayout->addWidget(pLabelPosx, 1, 0, 1, 1);
		pGridLayout->addWidget(m_pEditPosx, 1, 1, 1, 1);
		pGridLayout->addWidget(pLabelPosy, 2, 0, 1, 1);
		pGridLayout->addWidget(m_pEditPosy, 2, 1, 1, 1);
		pGridLayout->addWidget(pLabelAngle, 3, 0, 1, 1);
		pGridLayout->addWidget(m_pEditAngle, 3, 1, 1, 1);
	}
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	if (pMainLayout)
	{
		pMainLayout->addLayout(pGridLayout);
		pMainLayout->addWidget(m_pBtnConfirm);
	}
	setLayout(pMainLayout);
}

void VehicleInfoWnd::InitSlot()
{
	connect(m_pBtnConfirm, &QPushButton::clicked, this, &VehicleInfoWnd::SlotBtnClickConfirm);
}

void VehicleInfoWnd::SlotBtnClickConfirm()
{
	if (m_pEditAngle)
	{
		QString strAngle = m_pEditAngle->text();
		if (!strAngle.isEmpty())
		{
			m_vehicleInfo.angle = strAngle.toDouble();
			if (m_vehicleInfo.angle < 0.0)
			{
				m_vehicleInfo.angle = PT_ALG::angle_to_normal(m_vehicleInfo.angle);
			}
		}
	}
	accept();
}

void VehicleInfoWnd::SetVehicleInfo(const VEHICLEINFO& vehicleInfo)
{
	m_vehicleInfo = vehicleInfo;
	ShowCtrlData();
}

void VehicleInfoWnd::GetVehicleInfo(VEHICLEINFO& vehicleInfo)
{
	vehicleInfo = m_vehicleInfo;
}

void VehicleInfoWnd::ShowCtrlData()
{
	if (m_pEditVehicleId)
	{
		m_pEditVehicleId->setText(QString::number(m_vehicleInfo.vehicle_id));
	}
	if (m_pEditPosx)
	{
		m_pEditPosx->setText(QString::number(m_vehicleInfo.pos_x));
	}
	if (m_pEditPosy)
	{
		m_pEditPosy->setText(QString::number(m_vehicleInfo.pos_y));
	}
	if (m_pEditAngle)
	{
		m_pEditAngle->setText(QString::number(m_vehicleInfo.angle));
	}
}