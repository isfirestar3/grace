#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qgridlayout.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qtableview.h>
#include <QtWidgets\qlabel.h>
#include <QtGui\QStandardItemModel>
#include <QtCore\QTimer>
#include <QtWidgets\QHeaderView>
#include <thread>
#include "os_util.hpp"
#include <atomic>
#include "vehicle.h"
#include "net_impls.h"
#include "config_xml.h"
#include "widget_delegate.hpp"
#include "push_button.h"

class wid_realtime_vehicle : public QWidget{
	Q_OBJECT

public:
	wid_realtime_vehicle(QWidget* parent = 0);
	~wid_realtime_vehicle();

public:
	void set_net_status(int&,int&);
	Q_INVOKABLE void UpdateBtnState(bool connect);
protected:
	void hideEvent(QHideEvent *);
private:
	void init_form();
	void init_slot();
	void init_thread();
	void init_data();

	void thread_read_vehicle();

private slots:
	void start_vehicle_clicked();
	void stop_vehicle_clicked();
	void pause_vehicle_clicked();
	void send_vehicle_clicked();
	void update_tabview();

private:
	PushButton* start_vehicle_monitor_{ nullptr };
	PushButton* stop_vehicle_monitor_{ nullptr };

	QTableView *vehicle_view_{ nullptr };
	QStandardItemModel* view_model_{ nullptr };
	QTimer* timer_{ nullptr };

	std::thread* read_vehicle_thread_{ nullptr };
	std::atomic<bool> read_flag = false;

	std::atomic<bool> is_thread_finish = false;

	int net_status_ = -1;
};