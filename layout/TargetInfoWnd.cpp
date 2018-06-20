#include "TargetInfoWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qradiobutton.h>
#include "DataManage.h"
#include <QtWidgets/qapplication.h>

TargetInfoWnd::TargetInfoWnd()
:m_iSelectWop(-1)
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


TargetInfoWnd::~TargetInfoWnd()
{
}

void TargetInfoWnd::InitWnd()
{
	resize(400, 450);
	setWindowTitle(en ? "TargetInfo" : QStringLiteral("选择目标点WOP"));
}

void TargetInfoWnd::InitCtrl()
{
	m_pWopTable = new QTableWidget;
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
	m_pBtnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
	m_pCheckBoxReloc = new QCheckBox(en ? "Reloc" : QStringLiteral("是否将目标点定位到线路上？"));
	if (m_pCheckBoxReloc)
	{
		m_pCheckBoxReloc->setChecked(true);
	}
}

void TargetInfoWnd::InitForm()
{
	QVBoxLayout* pWopLayout = new QVBoxLayout;
	if (pWopLayout)
	{
		QLabel* pWopLabel = new QLabel(en ? "Wop:" : QStringLiteral("WOP选择:"));
		pWopLayout->addWidget(pWopLabel);
		pWopLayout->addWidget(m_pWopTable);
		pWopLayout->addWidget(m_pCheckBoxReloc);
		pWopLayout->addWidget(m_pBtnConfirm);
	}
	setLayout(pWopLayout);
}

void TargetInfoWnd::InitSlot()
{
	connect(m_pBtnConfirm, &QPushButton::clicked, this, &TargetInfoWnd::SlotBtnConfirm);
}

void TargetInfoWnd::SlotBtnConfirm()
{
	if (!m_pWopTable)
	{
		return;
	}
	int iItemCount = m_pWopTable->rowCount();
	for (int i = 0; i < iItemCount; ++i)
	{
		QRadioButton* pRadio = reinterpret_cast<QRadioButton*>(m_pWopTable->cellWidget(i, 4));
		if (pRadio && pRadio->isChecked())
		{
			QTableWidgetItem* pWopId = m_pWopTable->item(i, 0);
			if (pWopId)
			{
				m_iSelectWop = pWopId->text().toInt();
				break;
			}
		}
	}
	if (m_pCheckBoxReloc && m_pCheckBoxReloc->isChecked())
	{
		m_bRelocation = true;
	}
	this->accept();
}

void TargetInfoWnd::UpdateWopTable(const EDGEINFO& edgeInfo)
{
	if (m_pWopTable)
	{
		m_pWopTable->clearContents();
		m_pWopTable->setRowCount(0);

		for (auto wopId : edgeInfo.wop_list)
		{
			WOP wop;
			DATAMNG->GetWopById(wopId, wop);
			AddWopToTableWidget(wop);
		}
	}
}

void TargetInfoWnd::AddWopToTableWidget(const WOP& wopInfo)
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
		qsDirection = en ? "Forward:" : QStringLiteral("正向");
	}
	else if (wopInfo.direction == BACKWARD)
	{
		qsDirection = en ? "Backward:" : QStringLiteral("反向");
	}
	QString qsAngleType;
	if (wopInfo.angle_type == AngleType::CONST_ANGLE)
	{
		qsAngleType = en ? "Const Angle:" : QStringLiteral("切向恒定角");
	}
	else if (wopInfo.angle_type == AngleType::GLOBAL_CONST_ANGLE)
	{
		qsAngleType = en ? "Global Const Angle:" : QStringLiteral("全局恒定角");
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
	QRadioButton* pRadio = new QRadioButton;

	m_pWopTable->setItem(iRows, 0, pItemWopId);
	m_pWopTable->setItem(iRows, 1, pItemDirection);
	m_pWopTable->setItem(iRows, 2, pItemAngleType);
	m_pWopTable->setItem(iRows, 3, pItemAngle);
	m_pWopTable->setCellWidget(iRows, 4, pRadio);
}