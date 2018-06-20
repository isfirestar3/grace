#include "EdgeInfoWnd.h"
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qlabel.h>
#include "SelectWopWnd.h"
#include "DataManage.h"
#include "AlgorithmDefine.hpp"
#include "WopAttrWnd.h"
#include <QtWidgets/qapplication.h>

EdgeInfoWnd::EdgeInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


EdgeInfoWnd::~EdgeInfoWnd()
{
}

void EdgeInfoWnd::InitWnd()
{
	resize(450, 500);
	setWindowTitle(en ? "EdgeInfo" : QStringLiteral("边属性编辑"));
}

void EdgeInfoWnd::InitCtrl()
{
	m_pWopTable = new QTableWidget;  //wop列表
	if (m_pWopTable)
	{
		m_pWopTable->setColumnCount(4);
		QStringList headerList;
		headerList.clear();
		headerList << (en ? "Id" : QStringLiteral("ID")) << (en ? "Position" : QStringLiteral("运动方向")) << (en ? "AnchorAngleType" : QStringLiteral("攻角类型")) << (en ? "AnchorAngle" : QStringLiteral("攻角"));
		m_pWopTable->setHorizontalHeaderLabels(headerList);
		m_pWopTable->setContextMenuPolicy(Qt::CustomContextMenu);
		m_pWopTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pWopTable->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pWopTable->setSelectionMode(QAbstractItemView::SingleSelection);
	}

	m_pBtnAddWop = new QPushButton(en ? "AddWop" : QStringLiteral("添加"));
	m_pBtnSaveInfo = new QPushButton(en ? "SaveInfo" : QStringLiteral("保存"));
	m_pBtnExit = new QPushButton(en ? "Exit" : QStringLiteral("取消"));
	m_pEditId = new QLineEdit;
	m_pEditStartNodeId = new QLineEdit;
	m_pEditEndNodeId = new QLineEdit;
	m_pEditDistance = new QLineEdit;
	m_pRealDisLabel = new QLineEdit;
	m_pEditSpeed = new QLineEdit;
	if (m_pEditId)
	{
		m_pEditId->setEnabled(false);
	}
	if (m_pEditStartNodeId)
	{
		m_pEditStartNodeId->setEnabled(false);
	}
	if (m_pEditEndNodeId)
	{
		m_pEditEndNodeId->setEnabled(false);
	}
	if (m_pEditDistance)
	{
		m_pEditDistance->setEnabled(false);
	}
}

void EdgeInfoWnd::InitForm()
{
	QGridLayout* pAttrLayout = new QGridLayout;
	if (pAttrLayout)
	{
		QLabel* pIdLabel = new QLabel(en ? "Id:" : QStringLiteral("ID："));
		QLabel* pStartLabel = new QLabel(en ? "Start Node Id:" : QStringLiteral("开始节点ID："));
		QLabel* pEndLabel = new QLabel(en ? "End Node Id:" : QStringLiteral("结束节点ID："));
		QLabel* pDistance = new QLabel(en ? "Distance:" : QStringLiteral("长度(m)："));
		QLabel*pRealDisLabel = new QLabel(en ? "Distance:" : QStringLiteral("搜路长度(m)："));
		QLabel* pSpeed = new QLabel(en ? "Speed:" : QStringLiteral("速度："));
		pAttrLayout->addWidget(pIdLabel, 0, 0, 1, 1);
		pAttrLayout->addWidget(m_pEditId, 0, 1, 1, 1);
		pAttrLayout->addWidget(pStartLabel, 1, 0, 1, 1);
		pAttrLayout->addWidget(m_pEditStartNodeId, 1, 1, 1, 1);
		pAttrLayout->addWidget(pEndLabel, 2, 0, 1, 1);
		pAttrLayout->addWidget(m_pEditEndNodeId, 2, 1, 1, 1);
		pAttrLayout->addWidget(pDistance, 3, 0, 1, 1);
		pAttrLayout->addWidget(m_pEditDistance, 3, 1, 1, 1);
		pAttrLayout->addWidget(pRealDisLabel, 4, 0, 1, 1);
		pAttrLayout->addWidget(m_pRealDisLabel, 4, 1, 1, 1);
		pAttrLayout->addWidget(pSpeed, 5, 0, 1, 1);
		pAttrLayout->addWidget(m_pEditSpeed, 5, 1, 1, 1);
	}
	QGridLayout* pWopTitleLayout = new QGridLayout;
	if (pWopTitleLayout)
	{
		QLabel* pLabel = new QLabel(en ? "Wop List:" : QStringLiteral("WOP列表"));
		pWopTitleLayout->addWidget(pLabel, 0, 0, 1, 1);
		pWopTitleLayout->addWidget(m_pBtnAddWop, 0, 4, 1, 1);
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
		pWopLayout->addLayout(pAttrLayout);
		pWopLayout->addLayout(pWopTitleLayout);
		pWopLayout->addWidget(m_pWopTable);
		pWopLayout->addLayout(pSaveLayout);
	}
	setLayout(pWopLayout);
}

void EdgeInfoWnd::InitSlot()
{
	connect(m_pBtnAddWop, &QPushButton::clicked, this, &EdgeInfoWnd::SlotBtnAddWop);
	connect(m_pBtnSaveInfo, &QPushButton::clicked, this, &EdgeInfoWnd::SlotBtnSaveData);
	connect(m_pBtnExit, &QPushButton::clicked, this, &QDialog::reject);
	connect(m_pWopTable, &QTableWidget::itemDoubleClicked, this, &EdgeInfoWnd::SlotWopItemDClicked);
}

void EdgeInfoWnd::SlotBtnAddWop()
{
	SelectWopWnd selectWopWnd;
	if (selectWopWnd.exec() == QDialog::Accepted)
	{
		selectWopWnd.GetSelectedWop(m_edgeInfo.wop_list);
		UpdateWopTable();
	}
}

void EdgeInfoWnd::UpdateWopTable()
{
	if (!m_pWopTable)
	{
		return;
	}
	m_pWopTable->clearContents();
	m_pWopTable->setRowCount(0);
	for (auto wopId : m_edgeInfo.wop_list)
	{
		WOP wop;
		if (DATAMNG->GetWopById(wopId, wop))
		{
			AddWopToTableWidget(wop);
		}
	}
}

void EdgeInfoWnd::AddWopToTableWidget(const WOP& wopInfo)
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
		qsDirection = (en ? "Forward" : QStringLiteral("正向"));
	}
	else if (wopInfo.direction == BACKWARD)
	{
		qsDirection = (en ? "Backward" : QStringLiteral("反向"));
	}
	QString qsAngleType;
	if (wopInfo.angle_type == AngleType::CONST_ANGLE)
	{
		qsAngleType = (en ? "Const Angle" : QStringLiteral("切向恒定角"));
	}
	else if (wopInfo.angle_type == AngleType::GLOBAL_CONST_ANGLE)
	{
		qsAngleType = (en ? "Global Const Angle" : QStringLiteral("全局恒定角"));
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

	m_pWopTable->setItem(iRows, 0, pItemWopId);
	m_pWopTable->setItem(iRows, 1, pItemDirection);
	m_pWopTable->setItem(iRows, 2, pItemAngleType);
	m_pWopTable->setItem(iRows, 3, pItemAngle);
}

void EdgeInfoWnd::SlotBtnSaveData()
{
	GetCtrlData();
	DATAMNG->UpdateEdgeInfo(m_edgeInfo);
	this->accept();
}

void EdgeInfoWnd::SetEdgeInfo(const EDGEINFO& edgeInfo)
{
	m_edgeInfo = edgeInfo;
	UpdateWopTable();
	UpdateCtrlData();
}

void EdgeInfoWnd::GetEdgeInfo(EDGEINFO& edgeInfo)
{
	edgeInfo = m_edgeInfo;
}

void EdgeInfoWnd::UpdateCtrlData()
{
	if (m_pEditId)
	{
		m_pEditId->setText(QString::number(m_edgeInfo.id));
	}
	if (m_pEditStartNodeId)
	{
		m_pEditStartNodeId->setText(QString::number(m_edgeInfo.start_node_id));
	}
	if (m_pEditEndNodeId)
	{
		m_pEditEndNodeId->setText(QString::number(m_edgeInfo.end_node_id));
	}
	if (m_pEditDistance)
	{
		m_pEditDistance->setText(QString::number(DataConvert::map2world(m_edgeInfo.length)));
	}
	if (m_pRealDisLabel)
	{
		m_pRealDisLabel->setText(QString::number(DataConvert::map2world(m_edgeInfo.reLength)));
	}
	if (m_pEditSpeed)
	{
		if (m_edgeInfo.speed.size() > 0)
		{
			m_pEditSpeed->setText(QString::number(m_edgeInfo.speed[0].speed));
		}
	}
}

void EdgeInfoWnd::GetCtrlData()
{
	if (m_pEditSpeed)
	{
		double speed = m_pEditSpeed->text().toDouble();
		if (m_edgeInfo.speed.size() > 0)
		{
			m_edgeInfo.speed[0].speed = speed;
		}
	}
	if (m_pRealDisLabel)
	{
		m_edgeInfo.reLength =DataConvert::world2map(m_pRealDisLabel->text().toDouble());
	}
}

void EdgeInfoWnd::SlotWopItemDClicked(QTableWidgetItem* pItem)
{
	if (!pItem){
		return;
	}
	int iRow = pItem->row();
	QTableWidgetItem* pIdItem = m_pWopTable->item(iRow, 0);
	int wopId = pIdItem->text().toInt();
	WOPATTR wopAttr;
	if (m_edgeInfo.wop_attr_list.size() == 0){//刚画的edge,没有wop属性,wop_attr_list的size为0,所以无法保存wop属性，需要初始化属性
		WOPATTR wop_attr;
		wop_attr.id = wopId;
		m_edgeInfo.wop_attr_list.push_back(wop_attr);
	}
	for (auto wop_attr : m_edgeInfo.wop_attr_list)
	{
		if (wop_attr.id == wopId){
			wopAttr = wop_attr;
		}
	}
	WopAttrWnd wopAttrWnd;
	wopAttrWnd.SetWopAttr(wopAttr);
	if (wopAttrWnd.exec() == QDialog::Accepted)
	{
		wopAttrWnd.GetWopAttr(wopAttr);
		for (auto &wop_attr : m_edgeInfo.wop_attr_list){
			if (wop_attr.id == wopId){
				wop_attr = wopAttr;
			}
		}
	}
}

