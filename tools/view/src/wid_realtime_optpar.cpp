#include "view_net.h"
#include "wid_realtime_optpar.h"
#include "push_button.h"
#include "log.h"
#include "dbg_vars_impls.h"
#include "mntypes.h"

extern agv_api agv_interface_single_;

namespace
{
	static const int TOOL_BTN_WIDTH = 32;
	static const int TOOL_BTN_HEIGHT = 32;
}

wid_realtime_optpar::wid_realtime_optpar(QWidget* parent)
{
	init_form();
	init_slot();
	init_thread();
	init_data();
}

wid_realtime_optpar::~wid_realtime_optpar()
{
	is_thread_finish = true;
	if (read_optpar_thread_)
	{
		if (read_optpar_thread_->joinable())
		{
			read_optpar_thread_->join();
		}
		delete read_optpar_thread_;
		read_optpar_thread_ = nullptr;
	}
}

void wid_realtime_optpar::init_form()
{
	start_optpar_monitor_ = new PushButton;
	stop_optpar_monitor_ = new PushButton;
	start_optpar_monitor_->SetPixmap(":/wid_operate/sys_start");
	stop_optpar_monitor_->SetPixmap(":/wid_operate/sys_stop");

	start_optpar_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));
	stop_optpar_monitor_->setFixedSize(QSize(TOOL_BTN_WIDTH, TOOL_BTN_HEIGHT));

	start_optpar_monitor_->setEnabled(false);
	stop_optpar_monitor_->setEnabled(false);

	view_model_ = new QStandardItemModel();
	view_model_->setColumnCount(2);

	view_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("struct name"));
	view_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("struct value"));

	optpar_tab_ = new QTableView;
	optpar_tab_->setModel(view_model_);
	optpar_tab_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	optpar_tab_->verticalHeader()->setHidden(true);
	optpar_tab_->setColumnWidth(0, 300);
	optpar_tab_->setColumnWidth(1, 400);

	//optpar_tab_->setItemDelegateForColumn(1, new empty_delegate(this));

	QWidget* wid_left = new QWidget;
	QHBoxLayout*h_layout = new QHBoxLayout;
	h_layout->addWidget(wid_left);
	h_layout->addWidget(start_optpar_monitor_);
	h_layout->addWidget(stop_optpar_monitor_);
	h_layout->setStretchFactor(wid_left, 3);
	h_layout->setStretchFactor(start_optpar_monitor_, 1);
	h_layout->setStretchFactor(stop_optpar_monitor_, 1);

	QVBoxLayout*v_layout = new QVBoxLayout;
	v_layout->addWidget(optpar_tab_);
	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);
	QPalette palette_tmp;
	palette_tmp.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette_tmp);

	timer_ = new QTimer(this);
}

void wid_realtime_optpar::init_slot()
{
	connect(start_optpar_monitor_, SIGNAL(clicked()), this, SLOT(start_optpar_clicked()));
	connect(stop_optpar_monitor_, SIGNAL(clicked()), this, SLOT(stop_optpar_clicked()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(update_tabview()));
}

void wid_realtime_optpar::init_data()
{
	std::vector < std::string > vct_str;
	nsp::toolkit::singleton<config_load>::instance()->get_optpar_struct(vct_str);
	for (int i = 0; i < vct_str.size(); i++)
	{
		view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
		view_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
	}
}

void wid_realtime_optpar::init_thread()
{
	try{
		if (!read_optpar_thread_)
		{
			read_optpar_thread_ = new std::thread(std::bind(&wid_realtime_optpar::thread_read_optpar, this));
		}
	}
	catch (...)
	{
		return;
	}

}

void wid_realtime_optpar::update_tabview()
{
	optpar_tab_->reset();
}

void wid_realtime_optpar::thread_read_optpar()
{
	QModelIndex index;
	QVariant var;
	nsp::os::waitable_handle w_read_optpar;

	while (w_read_optpar.wait(nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time()))
	{
		if (is_thread_finish) break;
		if (!read_flag) continue;
		if (net_status_ < 0)continue;
 
		var__operation_parameter_t optpar_data_;
		if (agv_interface_single_.get_var_info_by_id_syn<var__operation_parameter_t>(kVarFixedObject_OptPar, optpar_data_) < 0)continue;

		int count = 0;
		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull00_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull01_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull02_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull03_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull04_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull05_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull06_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull07_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull08_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull09_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull10_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull11_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull12_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull13_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull14_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull15_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull16_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull17_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull18_;
		view_model_->setData(index, var);

		index = view_model_->index(count++, 1, QModelIndex());
		var = optpar_data_.ull19_;
		view_model_->setData(index, var);
	
	}
}

void wid_realtime_optpar::start_optpar_clicked()
{
	if (nullptr == timer_)
	{
		return;
	}

	start_optpar_monitor_->setEnabled(false);
	stop_optpar_monitor_->setEnabled(true);
	read_flag = true;
	int interval = nsp::toolkit::singleton<dbg_vars_impls>::instance()->get_refresh_time();
	timer_->start(interval);
}

void wid_realtime_optpar::stop_optpar_clicked()
{
	if (nullptr == timer_)
	{
		return;
	}

	start_optpar_monitor_->setEnabled(true);
	stop_optpar_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive())
	{
		timer_->stop();
	}
}


void wid_realtime_optpar::set_net_status(int&status,int& robot_id)
{
	//if (kNetworkStatus_Closed == status && stop_optpar_monitor_->isEnabled()){
	//	stop_optpar_monitor_->clicked();
	//}
	net_status_ = status;
	bool connect = false;
	if (mn::kNetworkStatus_Connected <= net_status_)
	{
		connect = true;
	}

	QMetaObject::invokeMethod(this, "UpdateBtnState", Qt::QueuedConnection,
		QGenericReturnArgument(),
		Q_ARG(bool, connect));
}

Q_INVOKABLE void wid_realtime_optpar::UpdateBtnState(bool connect)
{
	if (nullptr == start_optpar_monitor_ || nullptr == stop_optpar_monitor_)
	{
		return;
	}

	start_optpar_monitor_->setEnabled(connect && !read_flag);
	stop_optpar_monitor_->setEnabled(connect && read_flag);
}

void wid_realtime_optpar::hideEvent(QHideEvent *)
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

	start_optpar_monitor_->setEnabled(connect);
	stop_optpar_monitor_->setEnabled(false);
	read_flag = false;

	if (timer_->isActive())
	{
		timer_->stop();
	}
}
