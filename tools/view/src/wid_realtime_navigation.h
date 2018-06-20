#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlabel.h>
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
#include "enmu_table.h"
#include "push_button.h"

class wid_realtime_navigation : public QWidget{
	Q_OBJECT

public:
	wid_realtime_navigation(QWidget* parent = 0);
	~wid_realtime_navigation();

public:
	void set_net_status(int&,int& robot_id);
	Q_INVOKABLE void UpdateBtnState(bool connect);
protected:
	void hideEvent(QHideEvent *);
private:
	void init_form();
	void init_slot();
	void init_data();
	void init_thread();

	void thread_read_navigation();

private slots:
void navi_context_entered(QStandardItem * item);
	void start_navigation_clicked();
	void stop_navigation_clicked();
	void pause_navigation_clicked();
	void send_navigation_clicked();
	void update_tabview();

private:
	PushButton* start_navigation_monitor_{ nullptr };
	PushButton* stop_navigation_monitor_{ nullptr };

	QTableView* navigation_tab_{ nullptr };
	QStandardItemModel *view_model_{ nullptr };
	QTimer* timer_{ nullptr };

	std::thread* read_navigation_thread_ = nullptr;
	std::atomic<bool> read_flag = false;

	std::atomic<bool> is_thread_finish = false;

	int net_status_ = -1;//网络状态

	btn_state current_btn_;
	std::map<int, double> buffer_map_;//缓存所有的数据
	std::map<int, double> send_map_;//需要发送的数据
};
