#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcheckbox.h>

class NodesEditWnd :
	public QDialog
{
public:
	NodesEditWnd();
	~NodesEditWnd();

	bool pSpin=false;
private:
	void InitWnd();
	void InitForm();
	void InitSlot();
	void SlotBtnSaveData();

	QCheckBox* m_pSpin = nullptr;
	QPushButton* m_pBtnSaveInfo = nullptr;
	QPushButton* m_pBtnExit = nullptr;
};

