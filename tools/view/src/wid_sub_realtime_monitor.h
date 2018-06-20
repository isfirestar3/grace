#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qgridlayout.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qtableview.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qlabel.h>
#include <QtGui\QStandardItemModel>
#include <QtCore\QTimer>
#include <QtWidgets\QHeaderView>
#include <thread>
#include "os_util.hpp"
#include <atomic>
#include "vehicle.h"
#include "config_xml.h"
#include "widget_delegate.hpp"
#include "data_type.h"
#include "enmu_table.h"
#include "log.h"

#define ELMO_BOX			1
#define DWHEEL_BOX			2
#define SWHEEL_BOX			3
#define SDDEX_BOX			4
#define MOOS_BOX		    5
#define ANGLE_ENCODER_BOX   6
#define DIO_BOX             7

struct realtime_item_type{
	int id;
	std::string desc;
	int type;
};

class wid_sub_realtime_moniter : public QWidget{
	Q_OBJECT

public:
	wid_sub_realtime_moniter(QWidget* parent = 0);
	~wid_sub_realtime_moniter();

public:
	void set_net_status(int&,int&);
	void set_all_xml_data(all_xml_collection& xml_info);
	void set_child_wid(const std::function<void(int, void*, std::string&)>& func);//注册父窗口界面回调函数
	void set_other_wid_selected_id(int id, std::string&);//父窗口分发给子界面数据

private:
	void init_form();
	void init_slot();
	void init_thread();
	void init_combobox_name();
	void thread_read_table();
	void read_elmo_date();
	void read_navigation_date();
	void read_vehicle_date();
	void read_moos_date();
	void read_angle_encoder_date();
	void read_dwheel_date();
	void read_swheel_date();
	void read_sddex_date();
	void read_dio_date();

	private slots:
	void start_clicked();
	void stop_clicked();
	void init_data();
	void update_tabview();
	void current_index_changed(int);

private:
	QPushButton* start_monitor_;
	QPushButton* stop_monitor_;
	QComboBox*table_name_;
	QComboBox*table_id_;
	QTableView *table_view_;
	QStandardItemModel* table_model_;
	QTimer* timer_;
	std::thread* read_thread_ = nullptr;
	std::atomic<bool> read_flag = false;

	std::atomic<bool> is_thread_finish = false;

	std::mutex table_mutex_;

	std::atomic<int> net_status_ = -1;
	nsp::os::waitable_handle __thread_permission;
	all_xml_collection all_xml_data_;
	std::map<int, realtime_item_type> map_combox_index_;

	std::function<void(int, void*, std::string&)> monitor_id_callback_;

	int other_page_current_id_ = 0;
	var__elmo_t elmo_data_;
	std::string string_data_;

	std::atomic<int> robot_id_;
};