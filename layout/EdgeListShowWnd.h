#pragma once
#include <QtWidgets\qwidget.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qtablewidget.h>
#include "BasicDefine.h"
#include <functional>

class EdgeListShowWnd : public QWidget
{
public:
	EdgeListShowWnd();
	~EdgeListShowWnd();
public:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void SetAllEdgesInfo(const std::vector<EDGEINFO>& vecEdgeInfo);
	void InsertItemToListTable(const EDGEINFO&edgeinfo);
	void SlotTableItemDClicked(QTableWidgetItem*);
	void get_wop_attr(long long int value8, QString& values);
	void SetCallback(const std::function<void(int)>& callback);
private:
	QTableWidget* m_pTableEdgeList = nullptr;
	std::function<void(int)> call_back_list_clicked = nullptr;
	bool en;
};

