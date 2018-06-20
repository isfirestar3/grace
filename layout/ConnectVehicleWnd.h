#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qcombobox.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class ConnectVehicleWnd : public QDialog
{
public:
	ConnectVehicleWnd();
	~ConnectVehicleWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void ShowAllVehicle();
private:
	void SlotBtnClickedConfirm();
public:
	void SetVehicleInfo(std::vector<VEHICLEINFO>& vecVehicle);
	bool GetSelectVehicle(VEHICLEINFO& vehicleInfo);
	void SetConnectVehicle(const VEHICLEINFO& vehicleInfo);
private:
	QComboBox* m_combConnectMode = nullptr;
	QComboBox* m_combSelectVehicle = nullptr;
	QLineEdit* m_editConnectState = nullptr;
	QPushButton* m_btnConfirm = nullptr;
	std::vector<VEHICLEINFO> m_vecVehicleInfo;
	VEHICLEINFO m_connectVehicle;
	bool en;
};

