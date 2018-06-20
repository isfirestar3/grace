#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\qlabel.h>
#include <QtWidgets\qtextedit.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qtableview.h>
#include <QtWidgets\qpushbutton.h>
#include <QtWidgets\qgridlayout.h>
#include <QtCore\QTimer>

#include "wid_realtime_navigation.h"
#include "wid_realtime_vehicle.h"
#include "wid_realtime_operation.h"
#include "wid_realtime_optpar.h"

#include <QtCore/QPropertyAnimation>
#include <QtCore/QSharedPointer>

struct ipport{
	std::string ip_;
	int port_;
	//connect_type_t type_;
};

class wid_specific_monitor : public QWidget{
	Q_OBJECT

public:
	wid_specific_monitor(QWidget* parent = 0);
	~wid_specific_monitor();

public:
	//void set_connect_callback(const std::function<void(void*, int,int,int)>& func);//主页面注册回调函数获取子页面网络连接状态
	//void set_current_netstatus(int net_status,int net_mode,int nets);
	void on_tab_index_changed(int index);
	void update_net_status(int status, int robot_id);
	void after_show();
private:
	void init_form();

	void set_net_status();
	void register_rece_callback(uint32_t id, void *data, int type);


private:
	//QComboBox* mode_combox_;
	//QComboBox* ip_combox_;
	//QPushButton* connect_btn_;
	QTabWidget* realtime_tab_{ nullptr };

	wid_realtime_navigation* realtime_navigation_{ nullptr };
	wid_realtime_vehicle* realtime_vehicle_{ nullptr };
	wid_realtime_operation* realtime_operation_{ nullptr };
	wid_realtime_optpar* realtime_optpar_{nullptr};

	int __net_status = mn::kNetworkStatus_Closed;//网络连接状态,初始状态使用未连接状态

	//std::function<void(void*, int,int,int)>  child_net_callback_;

	int robot_id_;//机器人id号
	QSharedPointer<QPropertyAnimation> pa;
};