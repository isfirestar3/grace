#include "view_net.h"
#include "wid_realtime_navigation.h"
#include "log.h"
#include "dbg_vars_impls.h"

extern agv_api agv_interface_single_;

namespace
{
	static const int TOOL_BTN_WIDTH = 32;
	static const int TOOL_BTN_HEIGHT = 32;
}

wid_realtime_navigation::wid_realtime_navigation(QWidget* parent)
{
	init_form();
	init_slot();
	init_thread();
	init_data();
}

wid_realtime_navigation::~wid_realtime_navigation()
{
	is_thread_finish = true;
	if (read_navigation_thread_)
	{
		if (read_navigation_thread_->joinable())
		{
			read_navigation_thread_->join();
		}
		delete read_navigation_thread_;
		read_navigation_thread_ = nullptr;
	}
}

void wid_realtime_navigation::hideEvent(QHideEvent *)
{
	if (nullptr == timer_)
	{
		return;
	}

	bool connect = false;
	if (mn::kNetworkStatus_Connected <= net_status_)
	{
		connect = true;
	}

	start_navigation_monitor_->setEnabled(connect);
	stop_navigation_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive()){
		timer_->stop();
	}

	current_btn_ = STOP_MONITOR;
}

void wid_realtime_navigation::init_form()
{
	start_navigation_monitor_ = new PushButton;
	stop_navigation_monitor_ = new PushButton;
	start_navigation_monitor_->SetPixmap(":/wid_operate/sys_start");
	stop_navigation_monitor_->SetPixmap(":/wid_operate/sys_stop");
	start_navigation_monitor_->setEnabled(false);
	stop_navigation_monitor_->setEnabled(false);

	start_navigation_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));
	stop_navigation_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));

	view_model_ = new QStandardItemModel();
	view_model_->setColumnCount(2);
	
	view_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("struct name"));
	view_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("struct value"));

	navigation_tab_ = new QTableView;
	navigation_tab_->setModel(view_model_);
	navigation_tab_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	navigation_tab_->verticalHeader()->setHidden(true);
	navigation_tab_->setColumnWidth(0, 300);
	navigation_tab_->setColumnWidth(1, 400);

	QWidget* wid_left = new QWidget;
	QHBoxLayout*h_layout = new QHBoxLayout;
	h_layout->addWidget(wid_left);
	h_layout->addWidget(start_navigation_monitor_);
	h_layout->addWidget(stop_navigation_monitor_);
	
	h_layout->setStretchFactor(wid_left, 5);
	h_layout->setStretchFactor(start_navigation_monitor_, 1);
	h_layout->setStretchFactor(stop_navigation_monitor_, 1);

	QVBoxLayout*v_layout = new QVBoxLayout;
	v_layout->addWidget(navigation_tab_);
	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);
	QPalette palette_tmp;
	palette_tmp.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette_tmp);

	timer_ = new QTimer(this);
	//timer_->start(1000);
}

void wid_realtime_navigation::init_slot()
{
	connect(view_model_, SIGNAL(itemChanged(QStandardItem *)), this, SLOT(navi_context_entered(QStandardItem *)));
	connect(start_navigation_monitor_, SIGNAL(clicked()), this, SLOT(start_navigation_clicked()));
	connect(stop_navigation_monitor_, SIGNAL(clicked()), this, SLOT(stop_navigation_clicked()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(update_tabview()));
}

void wid_realtime_navigation::init_data()
{
	std::vector < std::string > vct_str;
	nsp::toolkit::singleton<config_load>::instance()->get_navigation_struct(vct_str);
	for (int i = 0; i < vct_str.size(); i++)
	{
		view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
		view_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
		view_model_->item(i, 0)->setEditable(false);
		//从第traj_ref_index_curr_字段之后的联合体内，不允许编辑下发
		if (i > 42){
			view_model_->item(i, 1)->setEditable(false);
			view_model_->item(i, 0)->setBackground(QBrush(QColor(200, 200, 200)));
			view_model_->item(i, 1)->setBackground(QBrush(QColor(200, 200, 200)));
		}
	}
}

void wid_realtime_navigation::init_thread()
{
	try{
		if (!read_navigation_thread_)
		{
			read_navigation_thread_ = new std::thread(std::bind(&wid_realtime_navigation::thread_read_navigation, this));
		}
	}
	catch (...)
	{
		return;
	}

}

void wid_realtime_navigation::update_tabview()
{
	navigation_tab_->reset();
}

void wid_realtime_navigation::thread_read_navigation()
{
	QModelIndex index;
	QVariant var;
	nsp::os::waitable_handle w_read_navigation;
	while (w_read_navigation.wait(nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time()))
	{
		if (is_thread_finish) break;
		if (!read_flag) continue;
		if (net_status_ < 0)continue;

		_var__navigation_t nav_data;
		if (agv_interface_single_.get_var_info_by_id_syn<_var__navigation_t>(kVarFixedObject_Navigation, nav_data) < 0)continue;

		int count = 0;
		//max_speed
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.max_speed_;
		view_model_->setData(index, var);
		//creep_speed
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.creep_speed_;
		view_model_->setData(index, var);
		//max_w
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.max_w_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.creep_w_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.slow_down_speed_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.acc_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dec_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dec_estop_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.acc_w_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dec_w_;
		view_model_->setData(index, var);
	
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.creep_distance_;
		view_model_->setData(index, var);
		//creep_theta
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.creep_theta_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.upl_mapping_angle_tolerance_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.upl_mapping_dist_tolerance_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.upl_mapping_angle_weight_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.upl_mapping_dist_weight_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.tracking_error_tolerance_dist_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.tracking_error_tolerance_angle_;
		view_model_->setData(index, var);
	
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_dist_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.predict_time_;
		view_model_->setData(index, var);
		//aim_angle_p
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_angle_p_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_angle_i_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_angle_d_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.stop_tolerance_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.stop_tolerance_angle_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.stop_point_trim_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_ey_p_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_ey_i_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.aim_ey_d_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		if (nav_data.track_status_.command_ >= 0 && nav_data.track_status_.command_ < 15)
		{
			var = status_table[nav_data.track_status_.command_];
			view_model_->setData(index, var);
		}
		//track_status: middle
		index = view_model_->index(count++, 1, QModelIndex());
		if (nav_data.track_status_.middle_ >= 0 && nav_data.track_status_.middle_ < 15)
		{
			var = status_table[nav_data.track_status_.middle_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		if (nav_data.track_status_.response_ >= 0 && nav_data.track_status_.response_ < 15)
		{
			var = status_table[nav_data.track_status_.response_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_upl_.edge_id_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_upl_.percentage_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_upl_.wop_id_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_pos_.x_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_pos_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.dest_pos_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.pos_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.pos_.y_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.pos_.angle_;
		view_model_->setData(index, var);
		//pos_time_stamp
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.pos_time_stamp_;
		view_model_->setData(index, var);
		//pos_confidence
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.pos_confidence_;
		view_model_->setData(index, var);
		//traj_ref_index_curr
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.traj_ref_index_curr_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.upl_.edge_id_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.upl_.percentage_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.upl_.angle_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.upl_.wop_id_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.tracking_error_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.base_point_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.base_point_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.base_point_.angle_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.aim_point_.x_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.aim_point_.y_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.aim_point_.angle_;
		view_model_->setData(index, var);
		//aim_heading_error
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.aim_heading_error_;
		view_model_->setData(index, var);
	
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.predict_point_.x_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.predict_point_.y_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.predict_point_.angle_;
		view_model_->setData(index, var);
		//predict_point_curvature
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.predict_point_curvature_;
		view_model_->setData(index, var);
	
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.on_last_segment_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.dist_to_dest_;
		view_model_->setData(index, var);
		
		index = view_model_->index(count++, 1, QModelIndex());
		var = nav_data.i.current_task_id_;
		view_model_->setData(index, var);
	}
}

void wid_realtime_navigation::start_navigation_clicked()
{
	start_navigation_monitor_->setEnabled(false);
	stop_navigation_monitor_->setEnabled(true);
	read_flag = true;
	int interval = nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time();
	timer_->start(interval);
	current_btn_ = START_MONITOR;
}

void wid_realtime_navigation::stop_navigation_clicked()
{
	start_navigation_monitor_->setEnabled(true);
	stop_navigation_monitor_->setEnabled(false);
	read_flag = false;
	if (timer_->isActive()){
		timer_->stop();
	}
	current_btn_ = STOP_MONITOR;
}

void wid_realtime_navigation::pause_navigation_clicked()
{
	start_navigation_monitor_->setEnabled(true);
	stop_navigation_monitor_->setEnabled(true);
	read_flag = false;
	if (timer_->isActive()){
		timer_->stop();
	}
	current_btn_ = PAUSE_MONITOR;
	for (auto i = 0; i < view_model_->rowCount(); i++){
		auto iter = buffer_map_.find(i);
		if (iter != buffer_map_.end()){
			iter->second = view_model_->item(i, 1)->data().toDouble();
		}
		else{
			buffer_map_.insert(std::make_pair(i, view_model_->item(i, 1)->data().toDouble()));
		}
	}
}

void wid_realtime_navigation::send_navigation_clicked()
{
	//将所有tableview中可编辑框背景色替换成白色
	current_btn_ = SEND_COMMAND;
	for (int i = 0; i < 43; i++){
		view_model_->item(i, 1)->setBackground(QBrush(QColor(255, 255, 255)));
	}
}

void wid_realtime_navigation::set_net_status(int&status, int& robot_id)
{
	net_status_ = status;
	/*if (mn::kNetworkStatus_Closed == status){
		if (nullptr != stop_navigation_monitor_)
		{
			stop_navigation_monitor_->clicked();
		}
	}
	else if (mn::kNetworkStatus_Connected == status){

	}*/

	bool connect = false;

	if (mn::kNetworkStatus_Connected <= status)
	{
		connect = true;
	}

	QMetaObject::invokeMethod(this, "UpdateBtnState", Qt::QueuedConnection,
		QGenericReturnArgument(),
		Q_ARG(bool, connect));
}

Q_INVOKABLE void wid_realtime_navigation::UpdateBtnState(bool connect)
{
	if (nullptr == start_navigation_monitor_ || nullptr == stop_navigation_monitor_)
	{
		return;
	}

	start_navigation_monitor_->setEnabled(connect && !read_flag);
	stop_navigation_monitor_->setEnabled(connect && read_flag);
}

void wid_realtime_navigation::navi_context_entered(QStandardItem * child_item)
{
	if (current_btn_ == PAUSE_MONITOR){
		double input_value = child_item->text().toDouble();
		auto iter = buffer_map_.find(child_item->row());
		if (input_value != iter->second){
			child_item->setBackground(QBrush(QColor(83, 255, 83)));
		}
	}
}
