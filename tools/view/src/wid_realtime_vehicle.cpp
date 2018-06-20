#include "view_net.h"
#include "wid_realtime_vehicle.h"
#include "log.h"
#include "enmu_table.h"
#include "dbg_vars_impls.h"

extern agv_api agv_interface_single_;

namespace
{
	static const int TOOL_BTN_WIDTH = 32;
	static const int TOOL_BTN_HEIGHT = 32;
}

wid_realtime_vehicle::wid_realtime_vehicle(QWidget* parent)
{
	init_form();
	init_slot();
	init_thread();
	init_data();
}

wid_realtime_vehicle::~wid_realtime_vehicle()
{
	is_thread_finish = true;
	if (read_vehicle_thread_)
	{
		if (read_vehicle_thread_->joinable())
		{
			read_vehicle_thread_->join();
		}
		delete read_vehicle_thread_;
		read_vehicle_thread_ = nullptr;
	}
}

void wid_realtime_vehicle::init_form()
{
	vehicle_view_ = new QTableView;
	view_model_ = new QStandardItemModel;
	view_model_->setColumnCount(2);

	view_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("struct name"));
	view_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("struct value"));

	vehicle_view_->setModel(view_model_);
	vehicle_view_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	vehicle_view_->verticalHeader()->setHidden(true);
	vehicle_view_->setColumnWidth(0, 300);
	vehicle_view_->setColumnWidth(1, 400);

	start_vehicle_monitor_ = new PushButton;
	stop_vehicle_monitor_ = new PushButton;
	start_vehicle_monitor_->SetPixmap(":/wid_operate/sys_start");
	stop_vehicle_monitor_->SetPixmap(":/wid_operate/sys_stop");

	start_vehicle_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));
	stop_vehicle_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));

	start_vehicle_monitor_->setEnabled(false);
	stop_vehicle_monitor_->setEnabled(false);

	QWidget* wid_left = new QWidget;
	QHBoxLayout* h_layout = new QHBoxLayout;

	h_layout->addWidget(wid_left);
	h_layout->addWidget(start_vehicle_monitor_);
	h_layout->addWidget(stop_vehicle_monitor_);
	h_layout->setStretchFactor(wid_left, 6);
	h_layout->setStretchFactor(start_vehicle_monitor_, 1);
	h_layout->setStretchFactor(stop_vehicle_monitor_, 1);

	QVBoxLayout*v_layout = new QVBoxLayout;
	v_layout->addWidget(vehicle_view_);
	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);
	QPalette palette_tmp;
	palette_tmp.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette_tmp);

	timer_ = new QTimer(this);
}

void wid_realtime_vehicle::init_slot()
{
	connect(start_vehicle_monitor_, SIGNAL(clicked()), this, SLOT(start_vehicle_clicked()));
	connect(stop_vehicle_monitor_, SIGNAL(clicked()), this, SLOT(stop_vehicle_clicked()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(update_tabview()));
}

void wid_realtime_vehicle::init_thread()
{
	try{
		if (!read_vehicle_thread_)
		{
			read_vehicle_thread_ = new std::thread(std::bind(&wid_realtime_vehicle::thread_read_vehicle, this));
		}
	}
	catch (...)
	{
		return;
	}
}

void wid_realtime_vehicle::init_data()
{
	std::vector < std::string > vct_str;
	nsp::toolkit::singleton<config_load>::instance()->get_vehicle_struct(vct_str);
	for (int i = 0; i < vct_str.size(); i++)
	{
		view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
		view_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
		view_model_->item(i, 0)->setEditable(false);
		//从第command_velocity_字段之后的联合体内，不允许编辑下发
		if (i > 20){
			view_model_->item(i, 1)->setEditable(false);
			view_model_->item(i, 0)->setBackground(QBrush(QColor(200, 200, 200)));
			view_model_->item(i, 1)->setBackground(QBrush(QColor(200, 200, 200)));
		}
	}
}

void wid_realtime_vehicle::update_tabview()
{
	vehicle_view_->reset();
}

void wid_realtime_vehicle::thread_read_vehicle()
{
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;

	while (w_read.wait(nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time()))
	{
		if (is_thread_finish)break;
		if (!read_flag)continue;
		if (net_status_ < 0)continue;

		_var__vehicle_t vehicle_data;
		if (agv_interface_single_.get_var_info_by_id_syn<_var__vehicle_t>(kVarFixedObject_Vehide, vehicle_data) < 0)continue;

		int count = 0;
		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.vehicle_type_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.vehicle_id_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.chassis_type_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_speed_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.creep_speed_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_acc_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_dec_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.creep_w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_acc_w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.max_dec_w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.steer_angle_error_tolerance_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.manual_velocity_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.manual_velocity_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.manual_velocity_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.stop_normal_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.stop_emergency_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.fault_stop_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.slow_down_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.enable_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		if (vehicle_data.control_mode_ >= 0 && vehicle_data.control_mode_ < 3)
		{
			var = vehicle_control_mode[vehicle_data.control_mode_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.command_velocity_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.command_velocity_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.command_velocity_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.ref_velocity_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.ref_velocity_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.ref_velocity_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_command_velocity_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_command_velocity_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_command_velocity_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_velocity_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_velocity_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.actual_velocity_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.odo_meter_.x_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.odo_meter_.y_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.odo_meter_.w_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.time_stamp_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.is_moving_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.normal_stopped_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.emergency_stopped_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.slow_done_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = vehicle_data.i.total_odo_meter_;
		view_model_->setData(index, var);
	}
}

void wid_realtime_vehicle::start_vehicle_clicked()
{
	start_vehicle_monitor_->setEnabled(false);
	stop_vehicle_monitor_->setEnabled(true);
	read_flag = true;
	int interval = nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time();
	timer_->start(interval);
}

void wid_realtime_vehicle::stop_vehicle_clicked()
{
	start_vehicle_monitor_->setEnabled(true);
	stop_vehicle_monitor_->setEnabled(false);
	read_flag = false;
	if (timer_->isActive()){
		timer_->stop();
	}
}

void wid_realtime_vehicle::set_net_status(int&status,int& robot_id)
{
	net_status_ = status;
	/*if (mn::kNetworkStatus_Closed == status && stop_vehicle_monitor_->isEnabled()){
		stop_vehicle_monitor_->clicked();
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

Q_INVOKABLE void wid_realtime_vehicle::UpdateBtnState(bool connect)
{
	if (nullptr == start_vehicle_monitor_ || nullptr == stop_vehicle_monitor_)
	{
		return;
	}

	start_vehicle_monitor_->setEnabled(connect && !read_flag);
	stop_vehicle_monitor_->setEnabled(connect && read_flag);
}

void wid_realtime_vehicle::hideEvent(QHideEvent *)
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

	start_vehicle_monitor_->setEnabled(connect);
	stop_vehicle_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive()){
		timer_->stop();
	}
}

void wid_realtime_vehicle::pause_vehicle_clicked(){
	start_vehicle_monitor_->setEnabled(true);
	stop_vehicle_monitor_->setEnabled(false);
	read_flag = false;
	if (timer_->isActive()){
		timer_->stop();
	}
}

void wid_realtime_vehicle::send_vehicle_clicked(){

}
