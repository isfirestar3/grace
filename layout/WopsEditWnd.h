#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qtablewidget.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qpushbutton.h>
#include "BasicDefine.h"

class WopsEditWnd : public QDialog
{
public:
	WopsEditWnd();
	~WopsEditWnd();
private:
	void InitWnd();
	void InitForm();
	void InitCtrl();
	void InitSlot();
	int GetWopInfo(WOP& wopInfo);
	void AddWopToTableWidget(const WOP& wopInfo);
	bool SaveLayoutFile();
	bool IsWopExist(const WOP& wop);
	void AddWopInfo(WOP& wop);
	int  GetMaxWopId();
public:
	void UpdateTableWidget();
	void SetLayoutFilePath(const std::string& strFilePath){ m_strFilePath = strFilePath; }
private:
	void SlotBtnClickedAddWop();
	void SlotBtnClickedExit();
	void SlotBtnClickedSave();
private:
	void closeEvent(QCloseEvent *event);
private:
	QTableWidget* m_pWopTable;
	QComboBox* m_pComboDirection;
	QComboBox* m_pComboAngleType;
	QLineEdit* m_pEditAngle;
	QPushButton* m_pBtnAddWop;
	QPushButton* m_pBtnCancel;
	QPushButton* m_pBtnSave;

	std::string m_strFilePath;
	bool m_bOpera;
	std::vector<WOP> m_vecWopInfo;//本地保存一份临时操作的wop列表
	bool en;
};

