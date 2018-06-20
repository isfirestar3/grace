#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class AnchorInfoWnd : public QDialog
{
public:
	AnchorInfoWnd();
	~AnchorInfoWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void AddWopToTableWidget(const WOP& wopInfo);
	void SetCtrlData(const ANCHORINFO& anchorInfo);
	void ShowOptsList();
	void AddOptToTableWidget(const OPTINFO& optInfo);
	bool GetOptInfo(int opt_id, OPTINFO& optInfo);
	bool UpdataOptInfo(const OPTINFO& optInfo);
	void GetCtrlData();
private:
	void SlotBtnConfirm();
	void SlotOptTableItemDClicked(QTableWidgetItem* pItem);
	void SlotBtnAddOpt();
	void SlotUpdatePercent(const QString &pos);
public:
	void UpdateWopTable(const EDGEINFO& edgeInfo);
	void SetAnchorInfo(const AnchorInfo& anchorInfo);
	int GetSelectWopId(){ return m_iSelectWop; }
	bool GetRelocFlag(){ return m_bRelocation; }
	QString GetAnchorAngle(){ return m_strAnchorAngle; }
	void GetAnchorInfo(ANCHORINFO& anchorInfo){ anchorInfo = m_anchorInfo; }
private:
	QLineEdit* m_pEditAnchorId = nullptr;
	QLineEdit* m_pEditAnchorPosx = nullptr;
	QLineEdit* m_pEdtiAnchorPosy = nullptr;
	QLineEdit* m_pEditAanchorAngle = nullptr;
	QLineEdit* m_pEditEdgeId = nullptr;
	QLineEdit* m_pEditPercent = nullptr;
	QLineEdit* m_pEditAOA = nullptr;
	QTableWidget* m_pWopTable = nullptr;
	QPushButton* m_pBtnConfirm = nullptr;
	QCheckBox* m_pCheckBoxReloc = nullptr;
	QTableWidget* m_pTableOpts = nullptr;
	QPushButton* m_pBtnAddOpt = nullptr;
	int m_iSelectWop = -1;
	bool m_bRelocation = false;  //是否重新定位目标点到边上
	ANCHORINFO m_anchorInfo;
	QString m_strAnchorAngle = ""; //停靠点的角度
	bool en;
};

