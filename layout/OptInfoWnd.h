#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class OptInfoWnd : public QDialog
{
public:
	OptInfoWnd();
	~OptInfoWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void ShowCtrlData();
	void GetCtrlData();
private:
	void SlotBtnClickConfirm();
	void SlotBtnClickGetOptParam();
	void SlotBtnSendOptParam();
public:
	void SetOptInfo(const OPTINFO& optInfo);
	void GetOptInfo(OPTINFO& optInfo){ optInfo = m_optInfo; }
private:
	QLineEdit* m_pOptId = nullptr;
	QLineEdit* m_pOptType = nullptr;
	std::vector<QLineEdit*> m_pOptParam;
	QPushButton* m_pBtnUpdate = nullptr;
	QPushButton* m_pBtnSend = nullptr;
	QPushButton* m_pBtnConfirm = nullptr;

	OPTINFO m_optInfo;
	bool en;
};

