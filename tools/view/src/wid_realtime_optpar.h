#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qgridlayout.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qtableview.h>
#include <QtGui\QStandardItemModel>
#include <QtWidgets\QHeaderView>
#include <QtCore\QTimer>
#include <atomic>
#include <thread>
#include "os_util.hpp"
#include "config_xml.h"
#include <QtWidgets\QItemDelegate>
#include "net_impls.h"
#include "widget_delegate.hpp"
#include "optpar.h"

#include "view_net.h"

class PushButton;

class wid_realtime_optpar : public QWidget{
	Q_OBJECT

public:
	wid_realtime_optpar(QWidget* parent = 0);
	~wid_realtime_optpar();

public:
	void set_net_status(int&,int&);
	Q_INVOKABLE void UpdateBtnState(bool connect);
protected:
	void hideEvent(QHideEvent *);
private:
	void init_form();
	void init_slot();
	void init_data();
	void init_thread();

	void thread_read_optpar();

	private slots:
	void start_optpar_clicked();
	void stop_optpar_clicked();
	void update_tabview();

private:
	PushButton* start_optpar_monitor_{ nullptr };
	PushButton* stop_optpar_monitor_{ nullptr };
	QTableView* optpar_tab_{ nullptr };
	QStandardItemModel *view_model_{ nullptr };
	QTimer* timer_{ nullptr };

	std::thread* read_optpar_thread_{ nullptr };
	std::atomic<bool> read_flag = false;

	int net_status_ = -1;

	std::atomic<bool> is_thread_finish = false;
};
