#pragma once

#include <QtWidgets/QDialog>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets\qpushbutton.h>
#include "BasicDefine.h"
#include "vartypes.h"

class LocationOfCurve :public QDialog
{
public:
	LocationOfCurve();
	~LocationOfCurve();
	EDGEINFO edgeInfo_;
	int id_=0;
	std::vector<CODEPOSXYA>vct_pos_;
private:
	void InitWnd();
	void InitForm();
	void InitSlot();
	void SlotBtnSaveData();

	QLineEdit* m_posCount = nullptr;
	QLineEdit* m_posDis = nullptr;
	QPushButton* m_pBtnSaveInfo = nullptr;
	QPushButton* m_pBtnExit = nullptr;
};

