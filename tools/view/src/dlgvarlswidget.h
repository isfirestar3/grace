#ifndef DLGVARLSWIDGET_H
#define DLGVARLSWIDGET_H

#include "ui_dlgvarlswidget.h"
#include "commonackmodel.h"
#include "vehicle.h"
#include <QtWidgets/QWidget>
#include <QtCore/QMultiMap>
#include <QtCore/QSharedPointer>
#include "mntypes.h"

class DlgVarLsWidget : public QWidget
{
	Q_OBJECT

public:
	DlgVarLsWidget(QWidget *parent = nullptr);
	~DlgVarLsWidget();

	void UpdateNetStatus(int &robotId, int &netStatus)
	{
		m_robotId = robotId;
		//m_netStatus = netStatus;
	}

	void InitUiData();
	void InitUi();
	void InitSlots();
	void PostDdgVarLsRequest();
	void PostDdgVarLsRequestFinished(bool successed);
	void PostCommonReadVehicleFinished(bool successed);
	void PostCommonReadNavigationFinished(bool successed);
	void InitTypeCmb(const QMultiMap<int, uint32_t> &map);
	void InitIdCmb(const QList<uint32_t> &values);
	void TypeCmbIndexChanged(int index);
	void IdCmbIndexChanged(int index);
private:
	Ui::DlgVarLsWidget ui;
	int m_robotId{ -1 };
	//int m_netStatus{ mn::kNetworkStatus_Closed };
	std::vector<mn::var_item> m_items;
	var__vehicle_t m_vehicle;
	var__navigation_t m_navigation;
	QMultiMap<int, uint32_t> m_typeIdMap;
	QSharedPointer<CommonAckModel> m_commonAckModel;
};

#endif // DLGVARLSWIDGET_H
