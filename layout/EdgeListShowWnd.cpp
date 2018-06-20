#include "EdgeListShowWnd.h"
#include <QtWidgets\qlayout.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qheaderview.h>
#include <QtWidgets/qapplication.h>

EdgeListShowWnd::EdgeListShowWnd()
{
	en = qApp->property("language").toInt() == 1;
	InitWnd();
	InitCtrl();
	InitForm();
	InitSlot();
}


EdgeListShowWnd::~EdgeListShowWnd()
{
}

void EdgeListShowWnd::InitWnd()
{
	setWindowTitle(en ? "EdgeList" : QStringLiteral("边查看器"));
}

void EdgeListShowWnd::InitCtrl()
{
	m_pTableEdgeList = new QTableWidget; 
	if (m_pTableEdgeList != NULL)
	{
		m_pTableEdgeList->setColumnCount(10);
		QStringList headerList;
		headerList.clear();
		headerList << (en ? "Id" : QStringLiteral("ID")) << (en ? "Start Node Id" : QStringLiteral("开始节点ID")) << (en ? "End Node Id" : QStringLiteral("结束节点ID")) <<
			(en ? "Length" : QStringLiteral("长度")) << (en ? "Speed" : QStringLiteral("速度")) << (en ? "WopId" : QStringLiteral("WOPID")) << (en ? "WopId_1" : QStringLiteral("WOP_1")) << (en ? "WopId_2" : QStringLiteral("WOP_2")) << (en ? "WopId_3" : QStringLiteral("WOP_3")) << (en ? "WopId_4" : QStringLiteral("WOP_4"));
		m_pTableEdgeList->setHorizontalHeaderLabels(headerList);
		m_pTableEdgeList->setContextMenuPolicy(Qt::CustomContextMenu);
		m_pTableEdgeList->setEditTriggers(QAbstractItemView::NoEditTriggers);
		m_pTableEdgeList->setSelectionBehavior(QAbstractItemView::SelectRows);
		m_pTableEdgeList->setSelectionMode(QAbstractItemView::SingleSelection);
		m_pTableEdgeList->setColumnWidth(0, 50);
		QObject::connect(m_pTableEdgeList->horizontalHeader(), SIGNAL(sectionClicked(int)), m_pTableEdgeList, SLOT(sortByColumn(int)));
	}
}

void EdgeListShowWnd::InitForm()
{
	QVBoxLayout* pMainLayout = new QVBoxLayout;
	if (pMainLayout){
		pMainLayout->addWidget(m_pTableEdgeList);
	}
	this->setLayout(pMainLayout);
}

void EdgeListShowWnd::InitSlot()
{
	connect(m_pTableEdgeList, &QTableWidget::itemClicked, this, &EdgeListShowWnd::SlotTableItemDClicked);
}

void EdgeListShowWnd::SetAllEdgesInfo(const std::vector<EDGEINFO>& vecEdgeInfo)
{
	if (m_pTableEdgeList == NULL)
	{
		return ;
	}
	m_pTableEdgeList->clearContents();
	m_pTableEdgeList->setRowCount(0);
	for (auto& iter : vecEdgeInfo){
		InsertItemToListTable(iter);
	}
}
void EdgeListShowWnd::InsertItemToListTable(const EDGEINFO&edgeinfo)
{
	if (m_pTableEdgeList == NULL){
		return;
	}
	int iRows = m_pTableEdgeList->rowCount();
	m_pTableEdgeList->insertRow(iRows);
	QString iEdgeId = QString::number(edgeinfo.id);
	QString iStartId = QString::number(edgeinfo.start_node_id);
	QString iEndId = QString::number(edgeinfo.end_node_id);
	QString dLengthId = QString::number(edgeinfo.length);
	int i=0;
	QString dSpeed;
	for (auto iter = edgeinfo.speed.begin(); iter != edgeinfo.speed.end(); iter++){
		i++;
		QString perSpeed = QString::number(iter->speed);
		if (i != edgeinfo.speed.size()){
			perSpeed = perSpeed + ',';
		}
		dSpeed = dSpeed + perSpeed;
	}
	QString dWop;

	QString lWopAttr1;
	QString lWopAttr2;
	QString lWopAttr3;
	QString lWopAttr4;
	i = 0;
	for (auto wopiter = edgeinfo.wop_list.begin(); wopiter != edgeinfo.wop_list.end(); wopiter++){
		i++;
		QString perWop = QString::number(*wopiter);
		if (i != edgeinfo.wop_list.size()){
			perWop = perWop + ',';
		}
		dWop = dWop + perWop;
	}

	for (auto attriter = edgeinfo.wop_attr_list.begin(); attriter != edgeinfo.wop_attr_list.end(); attriter++){
		if (attriter->values.size() == 9){
		QString per_value;
		get_wop_attr(attriter->values[8], per_value);
		QString perWopAttr = QString::number(attriter->values[0]) + ',' + QString::number(attriter->values[1]) + ',' + QString::number(attriter->values[2]) + ',' + QString::number(attriter->values[3]) + ',' +
		QString::number(attriter->values[4]) + ',' + QString::number(attriter->values[5]) + ',' + QString::number(attriter->values[6]) + ',' + QString::number(attriter->values[7]) + ';' + per_value;
			if (attriter->id == 1){
			lWopAttr1 = perWopAttr;
			}
			else if (attriter->id == 2){
			lWopAttr2 = perWopAttr;
			}
			else if (attriter->id == 3){
			lWopAttr3 = perWopAttr;
			}
			else if (attriter->id == 4){
			lWopAttr4 = perWopAttr;
			}
		}

	}
	QTableWidgetItem* pItemEdgeId = new QTableWidgetItem;
	pItemEdgeId->setData(Qt::EditRole, edgeinfo.id);
	QTableWidgetItem* pItemStartId = new QTableWidgetItem;
	pItemStartId->setData(Qt::EditRole, edgeinfo.start_node_id);
	QTableWidgetItem* pItemEndId = new QTableWidgetItem;
	pItemEndId->setData(Qt::EditRole, edgeinfo.end_node_id);
	QTableWidgetItem* pItemLengthId = new QTableWidgetItem;
	pItemLengthId->setData(Qt::EditRole, edgeinfo.length);
	QTableWidgetItem* pItemSpeedId = new QTableWidgetItem(dSpeed);
	QTableWidgetItem* pItemWopId = new QTableWidgetItem(dWop);
	QTableWidgetItem* pItemWopAttr1 = new QTableWidgetItem(lWopAttr1);
	QTableWidgetItem* pItemWopAttr2 = new QTableWidgetItem(lWopAttr2);
	QTableWidgetItem* pItemWopAttr3 = new QTableWidgetItem(lWopAttr3);
	QTableWidgetItem* pItemWopAttr4 = new QTableWidgetItem(lWopAttr4);

	m_pTableEdgeList->setItem(iRows, 0, pItemEdgeId);
	m_pTableEdgeList->setItem(iRows, 1, pItemStartId);
	m_pTableEdgeList->setItem(iRows, 2, pItemEndId);
	m_pTableEdgeList->setItem(iRows, 3, pItemLengthId);
	m_pTableEdgeList->setItem(iRows, 4, pItemSpeedId);
	m_pTableEdgeList->setItem(iRows, 5, pItemWopId);
	m_pTableEdgeList->setItem(iRows, 6, pItemWopAttr1);
	m_pTableEdgeList->setItem(iRows, 7, pItemWopAttr2);
	m_pTableEdgeList->setItem(iRows, 8, pItemWopAttr3);
	m_pTableEdgeList->setItem(iRows, 9, pItemWopAttr4);

	//m_pTableEdgeList->resizeColumnsToContents();
	return;
}

void EdgeListShowWnd::SlotTableItemDClicked(QTableWidgetItem* pItem)
{
	if (pItem == NULL){
		return;
	}
	int select_row = pItem->row();
	QTableWidgetItem* item_id = m_pTableEdgeList->item(select_row, 0);
	int edge_id = item_id->text().toInt();

	if (call_back_list_clicked){
		call_back_list_clicked(edge_id);
	}
}

void EdgeListShowWnd::SetCallback(const std::function<void(int)>& callback)
{
	call_back_list_clicked = callback;
}

void EdgeListShowWnd::get_wop_attr(long long int value8, QString& values)
{
	int value;
	QString per_values;
	for (int i = 0; i < 32; ++i){
		if (((value8 >> (31 - i)) & 1) == 1){
			value = i;
			per_values = QString::number(value);
			per_values = per_values + ',';
			values = values + per_values;
		}
	}
}