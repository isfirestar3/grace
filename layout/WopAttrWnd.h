#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcheckbox.h>
#include "BasicDefine.h"

class WopAttrWnd : public QDialog
{
public:
	WopAttrWnd();
	~WopAttrWnd();
public:
	void InitWnd();
	void InitForm();
	void InitCtrl();
	void InitSlot();
	void SetWopAttr(const WOPATTR& wop_attr);
	void GetWopAttr(WOPATTR& wop_attr){ wop_attr = m_wop_attr; }
public:
	void SlotBtnConfirmClicked();
	void SlotBtnCancelClicked();
private:
	QLineEdit* m_editValue[8];
	QCheckBox* m_checkbox[32];
	QCheckBox* m_checkbox_enable;
	QPushButton* m_btnConfirm;
	QPushButton* m_btnCancel;

	WOPATTR m_wop_attr;
	bool en;
};

