#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class CtrlPointInfoWnd : public QDialog
{
public:
	CtrlPointInfoWnd();
	~CtrlPointInfoWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
private:
	void SlotBtnConfirm();
public:
	void SetPos(const QPointF& pos);
	void GetPos(QPointF& pos){ pos = m_currentPos; };
private:
	bool GetCtrlPos(QPointF& pos);
private:
	QLineEdit* m_pEditPosx = nullptr;
	QLineEdit* m_pEditPosy = nullptr;
	QPushButton* m_pBtnConfirm = nullptr;
	QPointF m_currentPos;
	bool en;
};