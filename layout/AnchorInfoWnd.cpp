#include "AnchorInfoWnd.h"
#include <QtWidgets/qlabel.h>
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qradiobutton.h>
#include "DataManage.h"
#include "AlgorithmDefine.hpp"
#include "OptInfoWnd.h"
#include <QtWidgets/qapplication.h>
#include "point_algorithm.hpp"


AnchorInfoWnd::AnchorInfoWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


AnchorInfoWnd::~AnchorInfoWnd()
{

}

void AnchorInfoWnd::InitWnd()
{
	resize(800, 450);
	setWindowTitle(en ? "AnchorInfo" : QStringLiteral("停靠点信息"));
}

void AnchorInfoWnd::InitCtrl()
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
	m_pTableOpts = new QTableWidget;
	if (m_pTableOpts)
	{
		m_pTableOpts->setColumnCount(3);
		QStringList headerList;
		headerList.clear();
		headerList << (en ? "Id" : QStringLiteral("ID")) << (en ? "Type" : QStringLiteral("类型")) << (en ? "Parameter" : QStringLiteral("参数"));
		m_pTableOpts->setHorizontalHeaderLabels(headerList);
		m_pTableOpts->setContextMenuPolicy(Qt::CustomContextMenu);
		m_pTableOpts->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pTableOpts->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pTableOpts->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pTableOpts->setColumnWidth(0, 50);
		m_pTableOpts->setColumnWidth(1, 80);
		m_pTableOpts->setColumnWidth(2, 220);
	}
	m_pBtnConfirm = new QPushButton(en ? "Confirm" : QStringLiteral("确定"));
	m_pBtnAddOpt = new QPushButton(en ? "AddOpt" : QStringLiteral("添加操作"));
	m_pCheckBoxReloc = new QCheckBox(en ? "Reloc" : QStringLiteral("是否将停靠点定位到线路上？"));
	if (m_pCheckBoxReloc)
	{
		m_pCheckBoxReloc->setChecked(false);
	}
	m_pEditAnchorId = new QLineEdit;
	m_pEditAnchorPosx = new QLineEdit;
	m_pEdtiAnchorPosy = new QLineEdit;
	m_pEditAanchorAngle = new QLineEdit;
	m_pEditEdgeId = new QLineEdit;
	m_pEditPercent = new QLineEdit;
	m_pEditAOA = new QLineEdit;
	if (m_pEditAnchorId)
	{
		m_pEditAnchorId->setEnabled(false);
	}
}

void AnchorInfoWnd::InitForm()
{
	QGridLayout* pGridLayout = new QGridLayout;
	if (pGridLayout)
	{
		QLabel* pLabelId = new QLabel(en ? "Id:" : QStringLiteral("ID："));
		QLabel* pLabelPosx = new QLabel(en ? "X:" : QStringLiteral("坐标x："));
		QLabel* pLabelPosy = new QLabel(en ? "Y:" : QStringLiteral("坐标y："));
		QLabel* pLabelAngle = new QLabel(en ? "Angle:" : QStringLiteral("角度："));
		QLabel* pLabelUpl = new QLabel(en ? "Upl:" : QStringLiteral("UPL信息"));
		QLabel* pLabelEdgeId = new QLabel(en ? "EdgeId:" : QStringLiteral("所在边ID："));
		QLabel* pLabelPercent = new QLabel(en ? "Percent:" : QStringLiteral("距离起点比例："));
		QLabel* pLabelAoa = new QLabel(en ? "Aoa:" : QStringLiteral("攻角："));
		pGridLayout->addWidget(pLabelId, 0, 0, 1, 1);
		pGridLayout->addWidget(m_pEditAnchorId, 0, 1, 1, 1);
		pGridLayout->addWidget(pLabelPosx, 1, 0, 1, 1);
		pGridLayout->addWidget(m_pEditAnchorPosx, 1, 1, 1, 1);
		pGridLayout->addWidget(pLabelPosy, 2, 0, 1, 1);
		pGridLayout->addWidget(m_pEdtiAnchorPosy, 2, 1, 1, 1);
		pGridLayout->addWidget(pLabelAngle, 3, 0, 1, 1);
		pGridLayout->addWidget(m_pEditAanchorAngle, 3, 1, 1, 1);
		pGridLayout->addWidget(pLabelUpl, 4, 0, 1, 2);
		pGridLayout->addWidget(pLabelEdgeId, 5, 0, 1, 1);
		pGridLayout->addWidget(m_pEditEdgeId, 5, 1, 1, 1);
		pGridLayout->addWidget(pLabelPercent, 6, 0, 1, 1);
		pGridLayout->addWidget(m_pEditPercent, 6, 1, 1, 1);
		pGridLayout->addWidget(pLabelAoa, 7, 0, 1, 1);
		pGridLayout->addWidget(m_pEditAOA, 7, 1, 1, 1);
	}
	QVBoxLayout* pWopLayout = new QVBoxLayout;
	if (pWopLayout)
	{
		QLabel* pWopLabel = new QLabel(en ? "Wop:" : QStringLiteral("WOP选择:"));
		pWopLayout->addLayout(pGridLayout);
		pWopLayout->addWidget(pWopLabel);
		pWopLayout->addWidget(m_pWopTable);
		pWopLayout->addWidget(m_pCheckBoxReloc);
		pWopLayout->addWidget(m_pBtnConfirm);
	}
	QVBoxLayout* pOptsLayout = new QVBoxLayout;
	if (pOptsLayout)
	{
		QLabel* pOptsLabel = new QLabel(en ? "Opts:" : QStringLiteral("操作列表："));
		pOptsLayout->addWidget(pOptsLabel);
		pOptsLayout->addWidget(m_pTableOpts);
		pOptsLayout->addWidget(m_pBtnAddOpt);
	}
	QHBoxLayout* pHLayout = new QHBoxLayout;
	if (pHLayout)
	{
		pHLayout->addLayout(pWopLayout);
		pHLayout->addLayout(pOptsLayout);
	}
	setLayout(pHLayout);
}

void AnchorInfoWnd::InitSlot()
{
	connect(m_pEditAnchorPosx, &QLineEdit::textChanged, this, &AnchorInfoWnd::SlotUpdatePercent);
	connect(m_pEdtiAnchorPosy, &QLineEdit::textChanged, this, &AnchorInfoWnd::SlotUpdatePercent);
	connect(m_pBtnConfirm, &QPushButton::clicked, this, &AnchorInfoWnd::SlotBtnConfirm);
	connect(m_pBtnAddOpt, &QPushButton::clicked, this, &AnchorInfoWnd::SlotBtnAddOpt);
	connect(m_pTableOpts, &QTableWidget::itemDoubleClicked, this, &AnchorInfoWnd::SlotOptTableItemDClicked);
}

void AnchorInfoWnd::SlotBtnConfirm()
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
	if (m_pEditAanchorAngle)
	{
		m_strAnchorAngle = m_pEditAanchorAngle->text();
	}
	GetCtrlData();
	this->accept();
}

void AnchorInfoWnd::UpdateWopTable(const EDGEINFO& edgeInfo)
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

void AnchorInfoWnd::AddWopToTableWidget(const WOP& wopInfo)
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

void AnchorInfoWnd::SetAnchorInfo(const AnchorInfo& anchorInfo)
{
	m_anchorInfo = anchorInfo;
	SetCtrlData(anchorInfo);
}

void AnchorInfoWnd::SetCtrlData(const ANCHORINFO& anchorInfo)
{
	if (m_pEditAnchorId)
	{
		m_pEditAnchorId->setText(QString::number(anchorInfo.id));
	}
	if (m_pEditAnchorPosx)
	{
		m_pEditAnchorPosx->setText(QString::number(DataConvert::map2world(anchorInfo.pos_x)));
	}
	if (m_pEdtiAnchorPosy)
	{
		m_pEdtiAnchorPosy->setText(QString::number(DataConvert::map2world(anchorInfo.pos_y)));
	}
	if (m_pEditAanchorAngle)
	{
		m_pEditAanchorAngle->setText(QString::number(anchorInfo.angle));
	}
	if (m_pEditEdgeId)
	{
		m_pEditEdgeId->setText(QString::number(anchorInfo.edge_id));
	}
	if (m_pEditPercent)
	{
		m_pEditPercent->setText(QString::number(anchorInfo.percent));
	}
	if (m_pEditAOA)
	{
		m_pEditAOA->setText(QString::number(anchorInfo.aoa));
	}
	ShowOptsList();
}

void AnchorInfoWnd::ShowOptsList()
{
	if (!m_pTableOpts)
	{
		return;
	}
	m_pTableOpts->clearContents();
	m_pTableOpts->setRowCount(0);
	for (auto opt : m_anchorInfo.opts)
	{
		AddOptToTableWidget(opt);
	}
}

void AnchorInfoWnd::AddOptToTableWidget(const OPTINFO& optInfo)
{
	if (!m_pTableOpts)
	{
		return;
	}
	int iRows = m_pTableOpts->rowCount();
	m_pTableOpts->insertRow(iRows);

	QString qsOptId = QString::number(optInfo.id);
	QString qsType = QString::number(optInfo.type);
	QString qsParams;
	for (size_t i = 0; i < OPT_PARAM_NUM; ++i)
	{
		if (i == 0)
		{
			qsParams = QString::number((double)((int64_t)optInfo.params[i]) / 1000);
		}
		else
		{
			qsParams += ("," + QString::number((double)((int64_t)optInfo.params[i]) / 1000));
		}
	}

	QTableWidgetItem* pItemId = new QTableWidgetItem(qsOptId);
	QTableWidgetItem* pItemType = new QTableWidgetItem(qsType);
	QTableWidgetItem* pItemParams = new QTableWidgetItem(qsParams);

	if (pItemParams)
	{
		pItemParams->setToolTip(qsParams);
	}

	m_pTableOpts->setItem(iRows, 0, pItemId);
	m_pTableOpts->setItem(iRows, 1, pItemType);
	m_pTableOpts->setItem(iRows, 2, pItemParams);
}

bool AnchorInfoWnd::GetOptInfo(int opt_id, OPTINFO& optInfo)
{
	for (auto opt : m_anchorInfo.opts)
	{
		if (opt_id == opt.id)
		{
			optInfo = opt;
			return true;
		}
	}
	return false;
}

bool AnchorInfoWnd::UpdataOptInfo(const OPTINFO& optInfo)
{
	if (optInfo.id <= 0)
	{
		return false;
	}
	bool bExist = false;
	for (auto &opt : m_anchorInfo.opts)
	{
		if (opt.id == optInfo.id)
		{
			opt = optInfo;
			bExist = true;
			break;
		}
	}
	if (!bExist)
	{
		m_anchorInfo.opts.push_back(optInfo);
	}
	return true;
}

void AnchorInfoWnd::SlotOptTableItemDClicked(QTableWidgetItem* pItem)
{
	if (!pItem)
	{
		return;
	}
	int iRow = pItem->row();
	QTableWidgetItem* pIdItem = m_pTableOpts->item(iRow, 0);
	int optId = pIdItem->text().toInt();
	OPTINFO optInfo;
	GetOptInfo(optId, optInfo);
	OptInfoWnd optInfoWnd;
	optInfoWnd.SetOptInfo(optInfo);
	if (optInfoWnd.exec() == QDialog::Accepted)
	{
		optInfoWnd.GetOptInfo(optInfo);
		UpdataOptInfo(optInfo);
		ShowOptsList();
	}
}

void AnchorInfoWnd::SlotBtnAddOpt()
{
	OptInfoWnd optInfoWnd;
	OPTINFO optInfo;
	optInfo.id = m_anchorInfo.opts.size() + 1;
	optInfoWnd.SetOptInfo(optInfo);
	if (optInfoWnd.exec() == QDialog::Accepted)
	{
		optInfoWnd.GetOptInfo(optInfo);
		UpdataOptInfo(optInfo);
		ShowOptsList();
	}
}

void AnchorInfoWnd::SlotUpdatePercent(const QString &pos)
{
	QLineEdit *pLineEdit = qobject_cast<QLineEdit *>(sender());

	if (nullptr == pLineEdit)
	{
		return;
	}

	const int factor = 100;

	if (pLineEdit == m_pEditAnchorPosx)
	{
		QString posY = m_pEdtiAnchorPosy->text();

		if (posY.isEmpty())
		{
			return;
		}

		m_anchorInfo.pos_x = pos.toDouble() * factor;
		m_anchorInfo.pos_y = posY.toDouble() * factor;
	}
	else if (pLineEdit == m_pEdtiAnchorPosy)
	{
		QString posX = m_pEditAnchorPosx->text();

		if (posX.isEmpty())
		{
			return;
		}

		m_anchorInfo.pos_x = posX.toDouble() * factor;
		m_anchorInfo.pos_y = pos.toDouble() * factor;
	}

	position_t pos_t;
	pos_t.x_ = m_anchorInfo.pos_x;
	pos_t.y_ = m_anchorInfo.pos_y;
	pos_t.angle_ = 0;
	UPL upl;
	PT_ALG::point_mapping_upl(pos_t, upl);//根据位置算UPL
	m_anchorInfo.percent = upl.percent;

	if (nullptr != m_pEditPercent)
	{
		m_pEditPercent->setText(QString::number(m_anchorInfo.percent));
	}
}

void AnchorInfoWnd::GetCtrlData()
{
	if (m_pEditEdgeId)
	{
		QString strEdgeId = m_pEditEdgeId->text();
		if (!strEdgeId.isEmpty())
		{
			m_anchorInfo.edge_id = strEdgeId.toInt();
		}
	}
	if (m_pEditPercent)
	{
		QString strPercent = m_pEditPercent->text();
		if (!strPercent.isEmpty())
		{
			m_anchorInfo.percent = strPercent.toDouble();
		}
	}
	if (m_pEditAOA)
	{
		QString strAoa = m_pEditAOA->text();
		if (!strAoa.isEmpty())
		{
			m_anchorInfo.aoa = strAoa.toDouble();
		}
	}
	if (m_pEditAnchorPosx)
	{
		QString strPosx = m_pEditAnchorPosx->text();
		if (!strPosx.isEmpty()){
			m_anchorInfo.pos_x = DataConvert::world2map(strPosx.toDouble());
		}
	}
	if (m_pEdtiAnchorPosy)
	{
		QString strPosy = m_pEdtiAnchorPosy->text();
		if (!strPosy.isEmpty()){
			m_anchorInfo.pos_y = DataConvert::world2map(strPosy.toDouble());
		}
	}
}
