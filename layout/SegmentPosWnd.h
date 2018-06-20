#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class SegmentPosWnd : public QDialog
{
public:
	SegmentPosWnd();
	~SegmentPosWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
private:
	void SlotBtnComfirm();
	void SlotBtnCancel();
public:
	void SetPos(const QPointF& pos);
	void GetPos(QPointF& pos);
private:
	QLineEdit* m_pEditPosx = nullptr;
	QLineEdit* m_pEditPosy = nullptr;
	QPushButton* m_pBtnComfirm = nullptr;
	QPushButton* m_pBtnCancel = nullptr;

	NodeInfo m_nodeInfo;
	bool en;
};