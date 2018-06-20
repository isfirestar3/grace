#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>

#include "BasicDefine.h"

class QCheckBox;
class QRcodeInfoWnd : public QDialog
{
public:
	QRcodeInfoWnd();
	~QRcodeInfoWnd();
public:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
public:
	void SetCodeInfo(const CODEPOSXYA& nodeInfo);
	void GetCodeInfo(CODEPOSXYA& nodeInfo);
private:
	void SlotBtnComfirm();
	void SlotBtnCancel();
private:
	QLineEdit* m_pEditeCodeId = nullptr;
	QLineEdit* m_pEditCodePosx = nullptr;
	QLineEdit* m_pEditCodePosy = nullptr;
	QLineEdit* m_pEditCodePosw = nullptr;
	QPushButton* m_pBtnComfirm = nullptr;
	QPushButton* m_pBtnCancel = nullptr;

	CODEPOSXYA m_posInfo;
	bool en;
};

