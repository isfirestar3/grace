#ifndef REALTIMEMONITORWIDGET_H
#define REALTIMEMONITORWIDGET_H

#include "ui_realtimemonitorwidget.h"
#include "dbgvarmodel.h"
#include "realtimemonitormodel.h"
#include <QtWidgets/QWidget>
#include <QtCore/QTimer>

class RealtimeMonitorWidget : public QWidget
{
	Q_OBJECT
public:
	enum class RMMonitorStatus
	{
		Rm_NoStatus,
		Rm_Start,
		Rm_Stop
	};
public:
	RealtimeMonitorWidget(QWidget *parent = nullptr);
	~RealtimeMonitorWidget();

	void InitUi();
	void InitSlots();
	void UpdateNetStatus(int &netStatus, int &robotId);
	void OnChooseDbgVar();
	void OnStartWatch();
	void OnStopWatch();
	void StartWatch();
	void StopWatch();

	Q_INVOKABLE void UpdateBtnState(bool enabled);
protected:
	void hideEvent(QHideEvent *);
private:
	Ui::RealtimeMonitorWidget ui;
	int m_netStatus{ mn::kNetworkStatus_Closed };
	RMMonitorStatus m_monitorStatus{ RMMonitorStatus::Rm_NoStatus };
	QMultiMap<QPair<int, uint32_t>, QString> m_choosedMap;
	QSharedPointer<RealtimeMonitorModel> m_rmModel;
	QSharedPointer<QTimer> m_watchTimer;
};

#endif // REALTIMEMONITORWIDGET_H
