#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets\qlineedit.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcheckbox.h>
#include "BasicDefine.h"

class EdgesEditWnd :
	public QDialog
{
public:
	EdgesEditWnd();
	~EdgesEditWnd();

private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
private:
	void SlotBtnAddWop();
	void SlotBtnSaveData();
	void UpdateWopTable();
	void AddWopToTableWidget(const WOP& wopInfo);
	void UpdateCtrlData();
	int GetCtrlData();
	void SlotWopItemDClicked(QTableWidgetItem* pItem);
public:
	void GetEdgeInfo(EDGEINFO& edgeInfo);
	void SetEdgeInfo(const EDGEINFO& edgeInfo);
private:
	QTableWidget* m_pWopTable = nullptr;
	QPushButton* m_pBtnAddWop = nullptr;
	QPushButton* m_pBtnSaveInfo = nullptr;
	QPushButton* m_pBtnExit = nullptr;
	QLineEdit* m_pEditSpeed = nullptr;

	EDGEINFO m_edgeInfo;

};

