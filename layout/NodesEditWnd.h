#pragma once
#include <QtWidgets/QDialog>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qcheckbox.h>
#include <QtWidgets\qlineedit.h>
#include <QtWidgets\qlabel.h>

class NodesEditWnd :
	public QDialog
{
public:
	NodesEditWnd();
	~NodesEditWnd();

	bool pSpin=false;
	bool pMove = false;
	double move_x=0;
	double move_y=0;
private:
	void InitWnd();
	void InitForm();
	void InitSlot();
	void SlotBtnSaveData();

	QCheckBox* m_pSpin = nullptr;
	QCheckBox* m_pMove = nullptr;
	QLabel* m_pLabel_X = nullptr;
	QLabel* m_pLabel_Y = nullptr;
	QLineEdit* m_pMove_X = nullptr;
	QLineEdit* m_pMove_Y = nullptr;
	QLabel* m_pCm_X = nullptr;
	QLabel* m_pCm_Y = nullptr;
	QPushButton* m_pBtnSaveInfo = nullptr;
	QPushButton* m_pBtnExit = nullptr;
};

