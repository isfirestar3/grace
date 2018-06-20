#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qtabwidget.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qgridlayout.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qcombobox.h>

#include "wid_sub_realtime_monitor.h"

class wid_realtime_monitor : public QWidget{
	Q_OBJECT

public:
	wid_realtime_monitor(QWidget* parent = 0);
	~wid_realtime_monitor();

public:
	void set_all_xml_data(all_xml_collection&);
	void get_all_xml_data(all_xml_collection&);
	void init_wid_item_data();

	void update_net_status(int status,int robot_id);

private:
	void init_form();

	void set_net_status();
	void register_rece_callback(uint32_t id, void *data, int type);
	void child_wid_monitor_id(int id, void*ptr, std::string&);

	wid_sub_realtime_moniter* sub_moniter_page1_;
	wid_sub_realtime_moniter* sub_moniter_page2_;
	wid_sub_realtime_moniter* sub_moniter_page3_;

	int __net_status = mn::kNetworkStatus_Closed;
	all_xml_collection all_xml_data_;

	int robot_id_;//»úÆ÷ÈËidºÅ
};