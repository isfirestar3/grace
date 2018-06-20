#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qpushbutton.h>
#include "BasicDefine.h"

class SelectWopWnd : public QDialog
{
public:
	SelectWopWnd();
	~SelectWopWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
private:
	void SlotBtnSaveSelectWop();
public:
	void UpdateTableWidget();
	void GetSelectedWop(std::vector<int>& vecSelectWop);
private:
	void AddWopToTableWidget(const WOP& wopInfo);
private:
	QTableWidget* m_pWopTable;
	QPushButton* m_pBtnSaveInfo;
	QPushButton* m_pBtnExit;
	std::vector<int> m_vecSelectWop;
	bool en;
};

