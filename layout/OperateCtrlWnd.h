#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class OperateCtrlWnd : public QDialog
{
public:
	OperateCtrlWnd();
	~OperateCtrlWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void GetCtrlData(OPTCTRLINFO& optCtrlInfo);
private:
	void SlotBtnClickSend();
private:
	QLineEdit* m_pOptCode = nullptr;
	std::vector<QLineEdit*> m_pOptParam;
	QPushButton* m_pBtnUpdate = nullptr;
	QPushButton* m_pBtnSend = nullptr;
	bool en;
};

