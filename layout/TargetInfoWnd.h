#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include "BasicDefine.h"

class TargetInfoWnd : public QDialog
{
public:
	TargetInfoWnd();
	~TargetInfoWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void AddWopToTableWidget(const WOP& wopInfo);
private:
	void SlotBtnConfirm();
public:
	void UpdateWopTable(const EDGEINFO& edgeInfo);
	int GetSelectWopId(){ return m_iSelectWop; }
	bool GetRelocFlag(){ return m_bRelocation; }
private:
	QTableWidget* m_pWopTable;
	QPushButton* m_pBtnConfirm;
	QCheckBox* m_pCheckBoxReloc;
	int m_iSelectWop;
	bool m_bRelocation = false;  //是否重新定位目标点到边上
	bool en;
};

