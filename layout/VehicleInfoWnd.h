#pragma once
#include <QtWidgets/qdialog.h>
#include <QtWidgets/qpushbutton.h>
#include <QtWidgets/qlineedit.h>
#include "BasicDefine.h"

class VehicleInfoWnd : public QDialog
{
public:
	VehicleInfoWnd();
	~VehicleInfoWnd();
private:
	void InitWnd();
	void InitCtrl();
	void InitForm();
	void InitSlot();
	void ShowCtrlData();
public:
	void SetVehicleInfo(const VEHICLEINFO& vehicleInfo);
	void GetVehicleInfo(VEHICLEINFO& vehicleInfo);
private:
	void SlotBtnClickConfirm();
private:
	QLineEdit* m_pEditAngle = nullptr;
	QLineEdit* m_pEditVehicleId = nullptr;
	QLineEdit* m_pEditPosx = nullptr;
	QLineEdit* m_pEditPosy = nullptr;
	QPushButton* m_pBtnConfirm = nullptr;

	VEHICLEINFO m_vehicleInfo;
	bool en;
};

