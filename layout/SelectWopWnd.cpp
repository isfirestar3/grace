#include "SelectWopWnd.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qcheckbox.h>
#include "DataManage.h"
#include <QtWidgets/qapplication.h>

SelectWopWnd::SelectWopWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
	UpdateTableWidget();
}


SelectWopWnd::~SelectWopWnd()
{
}

void SelectWopWnd::InitWnd()
{
	resize(400, 400);
	setWindowTitle(en ? "SelectWop" : QStringLiteral("WOP选择窗口"));
}

void SelectWopWnd::InitCtrl()
{
	m_pWopTable = new QTableWidget;  //wop列表
	if (m_pWopTable)
	{
		m_pWopTable->setColumnCount(5);
		QStringList headerList;
		headerList.clear();
		headerList << (en ? "Id" : QStringLiteral("ID")) << (en ? "Position" : QStringLiteral("运动方向")) << (en ? "AnchorAngleType" : QStringLiteral("攻角类型")) << (en ? "AnchorAngle" : QStringLiteral("攻角")) << (en ? "Choice" : QStringLiteral("选择"));
		m_pWopTable->setHorizontalHeaderLabels(headerList);
		m_pWopTable->setContextMenuPolicy(Qt::CustomContextMenu);
		m_pWopTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pWopTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pWopTable->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pWopTable->setColumnWidth(0, 50);
		m_pWopTable->setColumnWidth(1, 80);
		m_pWopTable->setColumnWidth(2, 110);
		m_pWopTable->setColumnWidth(3, 60);
		m_pWopTable->setColumnWidth(4, 50);
	}

	m_pBtnSaveInfo = new QPushButton(en ? "SaveInfo" : QStringLiteral("确定"));
	m_pBtnExit = new QPushButton(en ? "Exit" : QStringLiteral("取消"));
}

void SelectWopWnd::InitForm()
{
	QGridLayout* pWopTitleLayout = new QGridLayout;
	if (pWopTitleLayout)
	{
		QLabel* pLabel = new QLabel(en ? "Wop List" : QStringLiteral("WOP列表"));
		pWopTitleLayout->addWidget(pLabel, 0, 0, 1, 1);
	}
	QGridLayout* pSaveLayout = new QGridLayout;
	if (pSaveLayout)
	{
		pSaveLayout->addWidget(m_pBtnSaveInfo, 0, 3, 1, 1);
		pSaveLayout->addWidget(m_pBtnExit, 0, 4, 1, 1);
	}
	QVBoxLayout* pWopLayout = new QVBoxLayout;
	if (pWopLayout)
	{
		pWopLayout->addLayout(pWopTitleLayout);
		pWopLayout->addWidget(m_pWopTable);
		pWopLayout->addLayout(pSaveLayout);
	}
	setLayout(pWopLayout);
}

void SelectWopWnd::InitSlot()
{
	connect(m_pBtnSaveInfo, &QPushButton::clicked, this, &SelectWopWnd::SlotBtnSaveSelectWop);
	connect(m_pBtnExit, &QPushButton::clicked, this, &QDialog::reject);
}

void SelectWopWnd::SlotBtnSaveSelectWop()
{
	if (!m_pWopTable)
	{
		return;
	}
	int iItemCount = m_pWopTable->rowCount();
	for (int i = 0; i < iItemCount; ++i)
	{
		QCheckBox* pCheckBox = reinterpret_cast<QCheckBox*>(m_pWopTable->cellWidget(i, 4));
		if (pCheckBox && pCheckBox->checkState() == Qt::CheckState::Checked)
		{
			QTableWidgetItem* pWopId = m_pWopTable->item(i, 0);
			if (pWopId)
			{
				int iWopId = pWopId->text().toInt();
				m_vecSelectWop.push_back(iWopId);
			}
		}
	}
	this->accept();
}

void SelectWopWnd::AddWopToTableWidget(const WOP& wopInfo)
{
	if (!m_pWopTable)
	{
		return;
	}
	int iRows = m_pWopTable->rowCount();
	m_pWopTable->insertRow(iRows);

	QString qsWopId = QString::number(wopInfo.id);
	QString qsDirection;
	if (wopInfo.direction == FORWARD)
	{
		qsDirection = en ? "Forward" : QStringLiteral("正向");
	}
	else if (wopInfo.direction == BACKWARD)
	{
		qsDirection = en ? "Backward" : QStringLiteral("反向");
	}
	QString qsAngleType;
	if (wopInfo.angle_type == AngleType::CONST_ANGLE)
	{
		qsAngleType = en ? "Const Angle" : QStringLiteral("切向恒定角");
	}
	else if (wopInfo.angle_type == AngleType::GLOBAL_CONST_ANGLE)
	{
		qsAngleType = en ? "Global Const Angle" : QStringLiteral("全局恒定角");
	}
	QString qsAngle = QString::number(wopInfo.angle);

	QTableWidgetItem* pItemWopId = new QTableWidgetItem(qsWopId);
	QTableWidgetItem* pItemDirection = new QTableWidgetItem(qsDirection);
	QTableWidgetItem* pItemAngleType = new QTableWidgetItem(qsAngleType);
	QTableWidgetItem* pItemAngle = new QTableWidgetItem(qsAngle);

	if (pItemWopId)
	{
		pItemWopId->setToolTip(qsWopId);
	}
	if (pItemDirection != NULL)
	{
		pItemDirection->setToolTip(qsDirection);
	}
	if (pItemAngleType)
	{
		pItemAngleType->setToolTip(qsAngleType);
	}
	if (pItemAngle)
	{
		pItemAngle->setToolTip(qsAngle);
	}
	QCheckBox* pCheckBox = new QCheckBox;

	m_pWopTable->setItem(iRows, 0, pItemWopId);
	m_pWopTable->setItem(iRows, 1, pItemDirection);
	m_pWopTable->setItem(iRows, 2, pItemAngleType);
	m_pWopTable->setItem(iRows, 3, pItemAngle);
	m_pWopTable->setCellWidget(iRows, 4, pCheckBox);
}

void SelectWopWnd::UpdateTableWidget()
{
	std::vector<WOP> vecWopInfo;
	DATAMNG->GetAllWopInfo(vecWopInfo);
	for (auto wopInfo : vecWopInfo)
	{
		AddWopToTableWidget(wopInfo);
	}
}

void SelectWopWnd::GetSelectedWop(std::vector<int>& vecSelectWop)
{
	vecSelectWop = m_vecSelectWop;
}
