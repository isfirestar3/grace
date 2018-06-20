#include "view_net.h"
#include "wid_realtime_operation.h"
#include "push_button.h"
#include "log.h"
#include "enmu_table.h"
#include "vartypes.h"
#include "dbg_vars_impls.h"

extern agv_api agv_interface_single_;

namespace
{
	static const int TOOL_BTN_WIDTH = 32;
	static const int TOOL_BTN_HEIGHT = 32;
}

wid_realtime_operation::wid_realtime_operation(QWidget* parent)
{
	init_form();
	init_slot();
	init_thread();
	init_data();
}

wid_realtime_operation::~wid_realtime_operation()
{
	is_thread_finish = true;
	if (read_operation_thread_)
	{
		if (read_operation_thread_->joinable())
		{
			read_operation_thread_->join();
		}
		delete read_operation_thread_;
		read_operation_thread_ = nullptr;
	}
}

void wid_realtime_operation::init_form()
{
	start_operation_monitor_ = new PushButton;
	stop_operation_monitor_ = new PushButton;
	start_operation_monitor_->SetPixmap(":/wid_operate/sys_start");
	stop_operation_monitor_->SetPixmap(":/wid_operate/sys_stop");

	start_operation_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));
	stop_operation_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));

	start_operation_monitor_->setEnabled(false);
	stop_operation_monitor_->setEnabled(false);

	view_model_ = new QStandardItemModel();
	view_model_->setColumnCount(2);

	view_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("struct name"));
	view_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("struct value"));

	operation_tab_ = new QTableView;
	operation_tab_->setModel(view_model_);
	operation_tab_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	operation_tab_->verticalHeader()->setHidden(true);
	operation_tab_->setColumnWidth(0, 300);
	operation_tab_->setColumnWidth(1, 400);

	//operation_tab_->setItemDelegateForColumn(1, new empty_delegate(this));

	QWidget* wid_left = new QWidget;
	QHBoxLayout*h_layout = new QHBoxLayout;
	h_layout->addWidget(wid_left);
	h_layout->addWidget(start_operation_monitor_);
	h_layout->addWidget(stop_operation_monitor_);
	h_layout->setStretchFactor(wid_left, 3);
	h_layout->setStretchFactor(start_operation_monitor_, 1);
	h_layout->setStretchFactor(stop_operation_monitor_, 1);

	QVBoxLayout*v_layout = new QVBoxLayout;
	v_layout->addWidget(operation_tab_);
	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);
	QPalette palette_tmp;
	palette_tmp.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette_tmp);

	timer_ = new QTimer(this);
}

void wid_realtime_operation::init_slot()
{
	connect(start_operation_monitor_, SIGNAL(clicked()), this, SLOT(start_operation_clicked()));
	connect(stop_operation_monitor_, SIGNAL(clicked()), this, SLOT(stop_operation_clicked()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(update_tabview()));
}

void wid_realtime_operation::init_data()
{
	std::vector < std::string > vct_str;
	nsp::toolkit::singleton<config_load>::instance()->get_operation_struct(vct_str);
	for (int i = 0; i < vct_str.size(); i++)
	{
		view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
		view_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
	}
}

void wid_realtime_operation::init_thread()
{
	try{
		if (!read_operation_thread_)
		{
			read_operation_thread_ = new std::thread(std::bind(&wid_realtime_operation::thread_read_operation, this));
		}
	}
	catch (...)
	{
		return;
	}

}

void wid_realtime_operation::update_tabview()
{
	operation_tab_->reset();
}

void wid_realtime_operation::thread_read_operation()
{
	QModelIndex index;
	QVariant var;
	nsp::os::waitable_handle w_read_operation;
	while (w_read_operation.wait(nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time()))
	{
		if (is_thread_finish) break;
		if (!read_flag) continue;
		if (net_status_ < 0)continue;

		var__operation_t operation_data_;
		if (agv_interface_single_.get_var_info_by_id_syn<var__operation_t>(kVarFixedObject_Operation, operation_data_) < 0)continue;

		int count = 0;
		index = view_model_->index(count++, 1, QModelIndex());
		if (operation_data_.status_.command_ >= 0 && operation_data_.status_.command_ < 15)
		{
			var = status_table[operation_data_.status_.command_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		if (operation_data_.status_.middle_ >= 0 && operation_data_.status_.middle_ < 15)
		{
			var = status_table[operation_data_.status_.middle_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		if (operation_data_.status_.response_ >= 0 && operation_data_.status_.response_ < 15)
		{
			var = status_table[operation_data_.status_.response_];
			view_model_->setData(index, var);
		}

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.code_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param0_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param1_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param2_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param3_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param4_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param5_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param6_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param7_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param8_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.param9_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param10_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param11_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param12_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param13_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param14_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param15_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param16_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param17_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param18_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.param19_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = operation_data_.i.current_task_id_;
		view_model_->setData(index, var);
	}
}

void wid_realtime_operation::start_operation_clicked()
{
	if (nullptr == timer_)
	{
		return;
	}

	start_operation_monitor_->setEnabled(false);
	stop_operation_monitor_->setEnabled(true);
	read_flag = true;
	int interval = nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time();
	timer_->start(interval);
}

void wid_realtime_operation::stop_operation_clicked()
{
	if (nullptr == timer_)
	{
		return;
	}

	start_operation_monitor_->setEnabled(true);
	stop_operation_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive())
	{
		timer_->stop();
	}
}

void wid_realtime_operation::set_net_status(int&status,int& robot_id)
{
	net_status_ = status;
	/*if (mn::kNetworkStatus_Closed == status && stop_operation_monitor_->isEnabled()){
		stop_operation_monitor_->clicked();
	}*/
	bool connect = false;
	if (mn::kNetworkStatus_Connected <= net_status_)
	{
		connect = true;
	}
	QMetaObject::invokeMethod(this, "UpdateBtnState", Qt::QueuedConnection,
		QGenericReturnArgument(),
		Q_ARG(bool, connect));
}

Q_INVOKABLE void wid_realtime_operation::UpdateBtnState(bool connect)
{
	if (nullptr == start_operation_monitor_ || nullptr == stop_operation_monitor_)
	{
		return;
	}

	start_operation_monitor_->setEnabled(connect && !read_flag);
	stop_operation_monitor_->setEnabled(connect && read_flag);
}

void wid_realtime_operation::hideEvent(QHideEvent *)
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

	start_operation_monitor_->setEnabled(connect);
	stop_operation_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive())
	{
		timer_->stop();
	}
}
