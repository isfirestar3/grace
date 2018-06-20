#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class EdgeInfoWnd : public QDialog
{
public:
	EdgeInfoWnd();
	~EdgeInfoWnd();
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
	void GetCtrlData();
	void SlotWopItemDClicked(QTableWidgetItem* pItem);
public:
	void SetEdgeInfo(const EDGEINFO& edgeInfo);
	void GetEdgeInfo(EDGEINFO& edgeInfo);
private:
	QTableWidget* m_pWopTable = nullptr;
	QPushButton* m_pBtnAddWop = nullptr;
	QPushButton* m_pBtnSaveInfo = nullptr;
	QPushButton* m_pBtnExit = nullptr;
	QLineEdit* m_pEditId = nullptr;
	QLineEdit* m_pEditStartNodeId = nullptr;
	QLineEdit* m_pEditEndNodeId = nullptr;
	QLineEdit* m_pEditDistance = nullptr;
	QLineEdit*m_pRealDisLabel = nullptr;
	QLineEdit* m_pEditSpeed = nullptr;

	EDGEINFO m_edgeInfo;
	bool en;
};

