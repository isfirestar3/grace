#include"wid_sub_realtime_monitor.h"
#include <iostream>

wid_sub_realtime_moniter::wid_sub_realtime_moniter(QWidget* parent)
{
	init_form();
	init_slot();
	init_thread();
	init_combobox_name();
}

wid_sub_realtime_moniter::~wid_sub_realtime_moniter()
{
	is_thread_finish = true;
	if (read_thread_)
	{
		if (read_thread_->joinable())
		{
			read_thread_->join();
		}
		delete read_thread_;
		read_thread_ = nullptr;
	}
}

void wid_sub_realtime_moniter::init_form()
{
	QLabel * ch_label = new QLabel(QString::fromLocal8Bit("选择："));
	table_name_ = new QComboBox;
	table_id_ = new QComboBox;
	QHBoxLayout* choose_h_layout = new QHBoxLayout;
	choose_h_layout->addWidget(ch_label);
	choose_h_layout->addWidget(table_name_);
	choose_h_layout->addWidget(table_id_);
	choose_h_layout->setStretchFactor(ch_label, 1);
	choose_h_layout->setStretchFactor(table_name_, 2);
	choose_h_layout->setStretchFactor(table_id_, 2);

	table_view_ = new QTableView;
	table_model_ = new QStandardItemModel;
	table_model_->setColumnCount(2);

	table_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("struct name"));
	table_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("struct value"));

	table_view_->setModel(table_model_);
	table_view_->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	table_view_->verticalHeader()->setHidden(true);
	table_view_->setColumnWidth(0, 300);
	table_view_->setColumnWidth(1, 400);

	//table_view_->setItemDelegateForColumn(1, new empty_delegate(this));

	start_monitor_ = new QPushButton;
	stop_monitor_ = new QPushButton;
	start_monitor_->setText(QStringLiteral("开始监测"));
	stop_monitor_->setText(QStringLiteral("停止监测"));
	stop_monitor_->setEnabled(false);

	QWidget* wid_left = new QWidget;
	QHBoxLayout*h_layout = new QHBoxLayout;
	h_layout->addWidget(wid_left);
	h_layout->addWidget(start_monitor_);
	h_layout->addWidget(stop_monitor_);
	h_layout->setStretchFactor(wid_left, 3);
	h_layout->setStretchFactor(start_monitor_, 1);
	h_layout->setStretchFactor(stop_monitor_, 1);

	QVBoxLayout*v_layout = new QVBoxLayout;
	v_layout->addLayout(choose_h_layout);
	v_layout->addWidget(table_view_);
	v_layout->addLayout(h_layout);
	this->setLayout(v_layout);
	QPalette palette_tmp;
	palette_tmp.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setPalette(palette_tmp);

	timer_ = new QTimer(this);
	timer_->start(1000);
}

void wid_sub_realtime_moniter::init_combobox_name()
{
	table_name_->insertItem(0, QString::fromLocal8Bit("请选择"));
	table_name_->insertItem(1, QString::fromLocal8Bit("elmo"));
	table_name_->insertItem(2, QString::fromLocal8Bit("dwheel"));
	table_name_->insertItem(3, QString::fromLocal8Bit("swheel"));
	table_name_->insertItem(4, QString::fromLocal8Bit("sdddex"));
	/*table_name_->insertItem(5, QString::fromLocal8Bit("navigation"));
	table_name_->insertItem(6, QString::fromLocal8Bit("vehicle"));*/
	table_name_->insertItem(5, QString::fromLocal8Bit("moos"));
	table_name_->insertItem(6, QString::fromLocal8Bit("angle encoder"));
	table_name_->insertItem(7, QString::fromLocal8Bit("dio"));
}

void wid_sub_realtime_moniter::init_slot()
{
	connect(table_name_, SIGNAL(currentIndexChanged(int)), this, SLOT(current_index_changed(int)));
	connect(table_name_, SIGNAL(currentIndexChanged(int)), this, SLOT(init_data()));
	connect(start_monitor_, SIGNAL(clicked()), this, SLOT(start_clicked()));
	connect(stop_monitor_, SIGNAL(clicked()), this, SLOT(stop_clicked()));
	connect(timer_, SIGNAL(timeout()), this, SLOT(update_tabview()));
}

void wid_sub_realtime_moniter::current_index_changed(int index)
{
	table_id_->clear();
	map_combox_index_.clear();
	int count = 0;
	switch (index)
	{
	case ELMO_BOX:
		for (auto &iter : all_xml_data_.vct_elmo_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = ELMO_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}
		break;
	case DWHEEL_BOX:
		for (auto &iter : all_xml_data_.vct_dwheel_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = DWHEEL_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}		
		break;
	case SWHEEL_BOX:
		for (auto &iter : all_xml_data_.vct_swheel_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = SWHEEL_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}
		break;
	case SDDEX_BOX:
		for (auto& iter : all_xml_data_.vct_sddex_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = SDDEX_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}
		break;
	case MOOS_BOX:
		for (auto& iter : all_xml_data_.vct_moos_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = MOOS_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}
		break;
	case ANGLE_ENCODER_BOX:
		for (auto& iter : all_xml_data_.vct_angle_encoder_collection_)
		{
			table_id_->insertItem(count, QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.type = ANGLE_ENCODER_BOX;
			tmp.desc = iter.name_;
			map_combox_index_.insert(std::make_pair(count, tmp));
			count++;
		}
		break;
	case DIO_BOX:
		for (auto&iter : all_xml_data_.vct_dio_collection_)
		{
			table_id_->insertItem(count,QString::fromLocal8Bit(iter.name_));
			realtime_item_type tmp;
			tmp.id = iter.id_;
			tmp.desc = iter.name_;
			tmp.type = DIO_BOX;
			map_combox_index_.insert(std::make_pair(count,tmp));
			count++;
		}
		break;
	default:
		break;
	}
}

void wid_sub_realtime_moniter::set_all_xml_data(all_xml_collection& xml_info)
{
	all_xml_data_ = xml_info;
}

void wid_sub_realtime_moniter::init_thread()
{
	try{
		if (!read_thread_)
		{
			read_thread_ = new std::thread(std::bind(&wid_sub_realtime_moniter::thread_read_table, this));
		}
	}
	catch (...)
	{
		return;
	}
}

void wid_sub_realtime_moniter::init_data()
{
	table_model_->clear();
	table_view_->setColumnWidth(0, 400);
	table_view_->setColumnWidth(1, 400);

	QStringList qlist;
	qlist << QStringLiteral("struct name") << QStringLiteral("struct value");
	table_model_->setHorizontalHeaderLabels(qlist);

	std::vector < std::string > vct_str;
	int type = table_name_->currentIndex();
	switch (type)
	{
	case ELMO_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_elmo_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}		
			break;
	case DWHEEL_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_dwheel_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}
		break;
	case SWHEEL_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_swheel_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}
		break;
	case SDDEX_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_sddex_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}
		break;
	case MOOS_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_moos_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}
		break;
	case ANGLE_ENCODER_BOX:
		{
			nsp::toolkit::singleton<config_load>::instance()->get_angle_encoder_struct(vct_str);
			for (int i = 0; i < vct_str.size(); i++)
			{
				table_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
				table_model_->setItem(i, 1, new QStandardItem(QString::fromLocal8Bit("0")));
			}
		}
		break;
	case DIO_BOX:
	{
		nsp::toolkit::singleton<config_load>::instance()->get_dio_struct(vct_str);
		for (int i = 0; i < vct_str.size(); i++)
		{
			table_model_->setItem(i,0,new QStandardItem(QString::fromLocal8Bit(vct_str[i].c_str())));
			table_model_->setItem(i,1,new QStandardItem(QString::fromLocal8Bit("0")));
		}
	}
	default:
		break;
	}
	
}

void wid_sub_realtime_moniter::update_tabview()
{
	table_view_->reset();
}

void wid_sub_realtime_moniter::thread_read_table()
{
	nsp::os::waitable_handle w_read;
	while (w_read.wait(1000))
	{
		if (is_thread_finish)break;
		if (!read_flag)continue;
		if (net_status_ < 0)continue;

		switch (table_name_->currentIndex())
		{
		case ELMO_BOX:
			read_elmo_date();
			break;
		case DWHEEL_BOX:
			read_dwheel_date();
			break;
		case SWHEEL_BOX:
			read_swheel_date();
			break;
		case SDDEX_BOX:
			read_sddex_date();
			break;
		case MOOS_BOX:
			read_moos_date();
			break;
		case ANGLE_ENCODER_BOX:
			read_angle_encoder_date();
			break;
		case DIO_BOX:
			read_dio_date();
		default:
			break;
		}
	}
}

void wid_sub_realtime_moniter::start_clicked()
{
	if (net_status_ < 0) return;
	start_monitor_->setEnabled(false);
	stop_monitor_->setEnabled(true);
	read_flag = true;
}

void wid_sub_realtime_moniter::stop_clicked()
{
	start_monitor_->setEnabled(true);
	stop_monitor_->setEnabled(false);
	read_flag = false;
	other_page_current_id_ = 0;
	std::string tmp = "";
	monitor_id_callback_(other_page_current_id_, this, tmp);//回调给主页面
}

void wid_sub_realtime_moniter::set_net_status(int&status,int& robot_id)
{
	robot_id_ = robot_id;
	net_status_ = status;
	if (net_status_ == kNetworkStatus_Closed && stop_monitor_->isEnabled()){
		stop_monitor_->clicked(true);
	}
}

void wid_sub_realtime_moniter::set_child_wid(const std::function<void(int, void*, std::string&)>& func)
{
	monitor_id_callback_ = func;
}

void wid_sub_realtime_moniter::set_other_wid_selected_id(int id, std::string& tmp_data)
{
	other_page_current_id_ = id;
	string_data_ = tmp_data;
}

void wid_sub_realtime_moniter::read_elmo_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__elmo_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
		{
			tmp.var_id_ = iter.second.id;
			break;
		}	
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__elmo_t ack_str;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_,vct_comm, 
			std::make_shared<motion::asio_partnet>([&](uint32_t robot_id, const void *data) {
			if (!data) {
				water.sig();
				return;
			}
			asio_data_ = *(motion::asio_data*)(data);
			if (asio_data_.get_err() < 0) {
				water.sig();
				return;
			}
			ack = *(common_ack*)(data);
			ack_str = *(var__elmo_t*)(ack.data.c_str());
			water.sig();
		}));
		if (retval < 0)
		{
			return;
		}

		water.wait();
		if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
		{
			nsperror << "failed to get elmo data";
			return;
		}
		monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
		std::cout << this << " post elmo command value" << std::endl;
	}
	else{
		std::cout << this << " shared elmo value" << std::endl;
		ack_str = *(var__elmo_t*)(string_data_.c_str());
	}
	QModelIndex index;
	QVariant var;

	index = table_model_->index(0, 1, QModelIndex());
	var = ack_str.candev_head_.canbus_;
	table_model_->setData(index, var);
	index = table_model_->index(1, 1, QModelIndex());
	var = ack_str.candev_head_.canport_;
	table_model_->setData(index, var);
	index = table_model_->index(2, 1, QModelIndex());
	var = ack_str.candev_head_.cannode_;
	table_model_->setData(index, var);
	index = table_model_->index(3, 1, QModelIndex());
	var = ack_str.candev_head_.latency_;
	table_model_->setData(index, var);
	index = table_model_->index(4, 1, QModelIndex());
	var = ack_str.candev_head_.merge_;
	table_model_->setData(index, var);
	index = table_model_->index(5, 1, QModelIndex());
	var = ack_str.profile_speed_;
	table_model_->setData(index, var);
	index = table_model_->index(6, 1, QModelIndex());
	var = ack_str.profile_acc_;
	table_model_->setData(index, var);
	index = table_model_->index(7, 1, QModelIndex());
	var = ack_str.profile_dec_;
	table_model_->setData(index, var);
	index = table_model_->index(8, 1, QModelIndex());

	if (ack_str.status_.command_ >= 0 && ack_str.status_.command_ < 15){
		var = status_table[ack_str.status_.command_];
		table_model_->setData(index, var);
	}

	index = table_model_->index(9, 1, QModelIndex());
	if (ack_str.status_.middle_ >= 0 && ack_str.status_.middle_ < 15){
		var = status_table[ack_str.status_.middle_];
		table_model_->setData(index, var);
	}
	
	index = table_model_->index(10, 1, QModelIndex());
	if (ack_str.status_.response_ >= 0 && ack_str.status_.response_ < 15){
		var = status_table[ack_str.status_.response_];
		table_model_->setData(index, var);
	}

	index = table_model_->index(11, 1, QModelIndex());

	if (ack_str.control_mode_ >= 0 && ack_str.control_mode_ < 4){
		var = elmo_control_mode[ack_str.control_mode_];
		table_model_->setData(index, var);
	}
	
	index = table_model_->index(12, 1, QModelIndex());
	var = ack_str.command_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(13, 1, QModelIndex());
	var = ack_str.command_position_;
	table_model_->setData(index, var);
	index = table_model_->index(14, 1, QModelIndex());
	var = ack_str.command_current_;
	table_model_->setData(index, var);
	index = table_model_->index(15, 1, QModelIndex());
	var = ack_str.enable_;
	table_model_->setData(index, var);
	index = table_model_->index(16, 1, QModelIndex());
	var = ack_str.i.node_state_;
	table_model_->setData(index, var);
	index = table_model_->index(17, 1, QModelIndex());
	var = ack_str.i.error_code_;
	table_model_->setData(index, var);
	index = table_model_->index(18, 1, QModelIndex());
	var = ack_str.i.time_stamp_;
	table_model_->setData(index, var);
	index = table_model_->index(19, 1, QModelIndex());
	var = ack_str.i.actual_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(20, 1, QModelIndex());
	var = ack_str.i.actual_position_;
	table_model_->setData(index, var);
	index = table_model_->index(21, 1, QModelIndex());
	var = ack_str.i.actual_current_;
	table_model_->setData(index, var);
	index = table_model_->index(22, 1, QModelIndex());
	var = ack_str.i.enabled_;
	table_model_->setData(index, var);
	index = table_model_->index(23, 1, QModelIndex());
	var = ack_str.i.di_;
	table_model_->setData(index, var);
	index = table_model_->index(24, 1, QModelIndex());
	var = ack_str.i.do_;
	table_model_->setData(index, var);
	index = table_model_->index(25, 1, QModelIndex());
}

void wid_sub_realtime_moniter::read_moos_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__moos_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
			tmp.var_id_ = iter.second.id;
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__moos_t moos_data_;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_,vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id,  const void *data) {
		if (!data) {
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)(data);
		if (asio_data_.get_err() < 0) {
			water.sig();
			return;
		}
		ack = *(common_ack*)(data);
		moos_data_ = *(var__moos_t*)(ack.data.c_str());
		water.sig();
	}));
	if (retval < 0)
	{
		return;
	}
	water.wait();
	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	{
		nsperror << "failed to get moos data";
		return;
	}
	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	std::cout << this << " post moos command value" << std::endl;
	}
	else{
		std::cout << this << " shared moos value" << std::endl;
		moos_data_ = *(var__moos_t*)(string_data_.c_str());
	}

	index = table_model_->index(0, 1, QModelIndex());
	var = moos_data_.candev_head_.canbus_;
	table_model_->setData(index, var);
	index = table_model_->index(1, 1, QModelIndex());
	var = moos_data_.candev_head_.canport_;
	table_model_->setData(index, var);
	index = table_model_->index(2, 1, QModelIndex());
	var = moos_data_.candev_head_.cannode_;
	table_model_->setData(index, var);
	index = table_model_->index(3, 1, QModelIndex());
	var = moos_data_.candev_head_.latency_;
	table_model_->setData(index, var);
	index = table_model_->index(4, 1, QModelIndex());
	var = moos_data_.candev_head_.merge_;
	table_model_->setData(index, var);
	index = table_model_->index(5, 1, QModelIndex());
	var = moos_data_.profile_speed_;
	table_model_->setData(index, var);
	index = table_model_->index(6, 1, QModelIndex());
	var = moos_data_.profile_acc_;
	table_model_->setData(index, var);
	index = table_model_->index(7, 1, QModelIndex());
	var = moos_data_.profile_dec_;
	table_model_->setData(index, var);
	index = table_model_->index(8, 1, QModelIndex());
	if (moos_data_.status_.command_ >= 0 && moos_data_.status_.command_ < 15)
	{
		var = status_table[moos_data_.status_.command_];
		table_model_->setData(index, var);
	}
	index = table_model_->index(9, 1, QModelIndex());
	if (moos_data_.status_.middle_ >= 0 && moos_data_.status_.middle_ < 15)
	{
		var = status_table[moos_data_.status_.middle_];
		table_model_->setData(index, var);
	}
	index = table_model_->index(10, 1, QModelIndex());
	if (moos_data_.status_.response_ >= 0 && moos_data_.status_.response_ < 15)
	{
		var = status_table[moos_data_.status_.response_];
		table_model_->setData(index, var);
	}
	index = table_model_->index(11, 1, QModelIndex());
	if (moos_data_.control_mode_ >= 0 && moos_data_.control_mode_ < 4)
	{
		var = elmo_control_mode[moos_data_.control_mode_];
		table_model_->setData(index, var);
	}

	index = table_model_->index(12, 1, QModelIndex());
	var = moos_data_.command_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(13, 1, QModelIndex());
	var = moos_data_.command_position_;
	table_model_->setData(index, var);
	index = table_model_->index(14, 1, QModelIndex());
	var = moos_data_.command_current_;
	table_model_->setData(index, var);
	index = table_model_->index(15, 1, QModelIndex());
	var = moos_data_.enable_;
	table_model_->setData(index, var);
	index = table_model_->index(16, 1, QModelIndex());
	var = moos_data_.i.state_;
	table_model_->setData(index, var);
	index = table_model_->index(17, 1, QModelIndex());
	var = moos_data_.i.error_code_;
	table_model_->setData(index, var);
	index = table_model_->index(18, 1, QModelIndex());
	var = moos_data_.i.time_stamp_;
	table_model_->setData(index, var);
	index = table_model_->index(19, 1, QModelIndex());
	var = moos_data_.i.actual_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(20, 1, QModelIndex());
	var = moos_data_.i.actual_position_;
	table_model_->setData(index, var);
	index = table_model_->index(21, 1, QModelIndex());
	var = moos_data_.i.actual_current_;
	table_model_->setData(index, var);
	index = table_model_->index(22, 1, QModelIndex());
	var = moos_data_.i.enabled_;
	table_model_->setData(index, var);
	index = table_model_->index(23, 1, QModelIndex());
	var = moos_data_.i.di_;
	table_model_->setData(index, var);
	index = table_model_->index(24, 1, QModelIndex());
	var = moos_data_.i.do_;
	table_model_->setData(index, var);
	index = table_model_->index(25, 1, QModelIndex());
	
}

void wid_sub_realtime_moniter::read_angle_encoder_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__angle_encoder_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
			tmp.var_id_ = iter.second.id;
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__angle_encoder_t angle_encoder_data_;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_, vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id,  const void *data) {
		if (!data) {
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)(data);
		if (asio_data_.get_err() < 0) {
			water.sig();
			return;
		}
		ack = *(common_ack*)(data);
		angle_encoder_data_ = *(var__angle_encoder_t*)(ack.data.c_str());
		water.sig();
	}));
	if (retval < 0)
	{
		return;
	}
	water.wait();
	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	{
		nsperror << "failed to get angle_encoder data";
		return;
	}
	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	std::cout << this << " post angle_encoder command value" << std::endl;
	}
	else{
		std::cout << this << " shared angle_encoder value" << std::endl;
		angle_encoder_data_ = *(var__angle_encoder_t*)(string_data_.c_str());
	}
	index = table_model_->index(0, 1, QModelIndex());
	var = angle_encoder_data_.candev_head_.canbus_;
	table_model_->setData(index, var);
	index = table_model_->index(1, 1, QModelIndex());
	var = angle_encoder_data_.candev_head_.canport_;
	table_model_->setData(index, var);
	index = table_model_->index(2, 1, QModelIndex());
	var = angle_encoder_data_.candev_head_.cannode_;
	table_model_->setData(index, var);
	index = table_model_->index(3, 1, QModelIndex());
	var = angle_encoder_data_.candev_head_.latency_;
	table_model_->setData(index, var);
	index = table_model_->index(4, 1, QModelIndex());
	var = angle_encoder_data_.candev_head_.merge_;
	table_model_->setData(index, var);
	index = table_model_->index(5, 1, QModelIndex());
	if (angle_encoder_data_.encoder_type_ >= 0 && angle_encoder_data_.encoder_type_ < 2){
		var = angle_encoder_type[angle_encoder_data_.encoder_type_];
		table_model_->setData(index, var);
	}
	index = table_model_->index(6, 1, QModelIndex());
	var = angle_encoder_data_.i.state_;
	table_model_->setData(index, var);
	index = table_model_->index(7, 1, QModelIndex());
	var = angle_encoder_data_.i.actual_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(8, 1, QModelIndex());
	var = angle_encoder_data_.i.error_code_;
	table_model_->setData(index, var);
	index = table_model_->index(9, 1, QModelIndex());
	var = angle_encoder_data_.i.time_stamp_;
	table_model_->setData(index, var);
	index = table_model_->index(10, 1, QModelIndex());
	
}

void wid_sub_realtime_moniter::read_dio_date()
{
	//common_t tmp;
	//tmp.length_ = sizeof(var__dio_t);
	//tmp.offset_ = 0;
	//for (auto&iter : map_combox_index_){
	//	if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
	//		tmp.var_id_ = iter.second.id;
	//}
	//std::vector<common_t>vct_comm;
	//vct_comm.push_back(tmp);
	//nsp::os::waitable_handle water;
	//QModelIndex index;
	//QVariant var;
	//motion::asio_data asio_data_;
	//var__dio_t dio_data_;
	//common_ack ack;
	//if (other_page_current_id_ != vct_comm[0].var_id_)
	//{
	//	int retval = post_common_read_request_by_id(robot_id_, vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id, const void *data) {
	//		if (!data) {
	//			water.sig();
	//			return;
	//		}
	//		asio_data_ = *(motion::asio_data*)(data);
	//		if (asio_data_.get_err() < 0) {
	//			water.sig();
	//			return;
	//		}
	//		ack = *(common_ack*)(data);
	//		dio_data_ = *(var__dio_t*)(ack.data.c_str());
	//		water.sig();
	//	}));
	//	if (retval < 0)
	//	{
	//		return;
	//	}
	//	water.wait();
	//	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	//	{
	//		loerr << "failed to get dio data";
	//		return;
	//	}
	//	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	//	std::cout << this << " post dio command value" << std::endl;
	//}
	//else{
	//	std::cout << this << " shared dio value" << std::endl;
	//	dio_data_ = *(var__dio_t*)(string_data_.c_str());
	//}

	//std::string str_list;
	//char ch_list[32];

	//index = table_model_->index(0, 1, QModelIndex());
	//var = dio_data_.candev_head_.canbus_;
	//table_model_->setData(index, var);
	//index = table_model_->index(1, 1, QModelIndex());
	//var = dio_data_.candev_head_.canport_;
	//table_model_->setData(index, var);
	//index = table_model_->index(2, 1, QModelIndex());
	//var = dio_data_.candev_head_.cannode_;
	//table_model_->setData(index, var);
	//index = table_model_->index(3, 1, QModelIndex());
	//var = dio_data_.candev_head_.latency_;
	//table_model_->setData(index, var);
	//index = table_model_->index(4, 1, QModelIndex());
	//var = dio_data_.candev_head_.merge_;
	//table_model_->setData(index, var);
	//index = table_model_->index(5, 1, QModelIndex());
	//if (dio_data_.status_.command_ >= 0 && dio_data_.status_.command_ < 15){
	//	var = status_table[dio_data_.status_.command_];
	//	table_model_->setData(index, var);
	//}
	//index = table_model_->index(6, 1, QModelIndex());
	//var = dio_data_.do_;
	//table_model_->setData(index,var);
	//
	//index = table_model_->index(6, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.ao_[i], ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(8, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aocfg_[i].norm_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(9, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aocfg_[i].scale_coef_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(10, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aocfg_[i].bias_coef_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(11, 1, QModelIndex());
	//var = dio_data_.i.bus_state_;
	//table_model_->setData(index, var);
	//index = table_model_->index(12, 1, QModelIndex());
	//var = dio_data_.i.error_code_;
	//table_model_->setData(index, var);
	//index = table_model_->index(13, 1, QModelIndex());
	//var = dio_data_.i.enabled_;
	//table_model_->setData(index, var);
	//index = table_model_->index(14, 1, QModelIndex());
	//var = dio_data_.i.di_;
	//table_model_->setData(index, var);
	//index = table_model_->index(15, 1, QModelIndex());
	//
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.i.ai_[i], ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(16, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aicfg_[i].norm_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(17, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aicfg_[i].scale_coef_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(18, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.aicfg_[i].bias_coef_, ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	//index = table_model_->index(19, 1, QModelIndex());
	//var = dio_data_.i.do2_;
	//table_model_->setData(index, var);

	//index = table_model_->index(20, 1, QModelIndex());
	//str_list.clear();
	//for (int i = 0; i < 16; i++){
	//	itoa(dio_data_.i.ao2_[i], ch_list, 10);
	//	if (str_list.empty()){
	//		str_list += ch_list;
	//		continue;
	//	}str_list += ",";
	//	str_list += ch_list;
	//}
	//var = str_list.c_str();
	//table_model_->setData(index, var);

	/*index = table_model_->index(21, 1, QModelIndex());
	str_list.clear();
	for (int i = 0; i < 16; i++){
		itoa(dio_data_.ao_norm2_[i], ch_list, 10);
		if (str_list.empty()){
			str_list += ch_list;
			continue;
		}str_list += ",";
		str_list += ch_list;
	}
	var = str_list.c_str();
	table_model_->setData(index, var);

	index = table_model_->index(22, 1, QModelIndex());
	str_list.clear();
	for (int i = 0; i < 16; i++){
		itoa(dio_data_.ao_scale_coef2_[i], ch_list, 10);
		if (str_list.empty()){
			str_list += ch_list;
			continue;
		}str_list += ",";
		str_list += ch_list;
	}
	var = str_list.c_str();
	table_model_->setData(index, var);

	index = table_model_->index(23, 1, QModelIndex());
	str_list.clear();
	for (int i = 0; i < 16; i++){
		itoa(dio_data_.ao_bias_coef2_[i], ch_list, 10);
		if (str_list.empty()){
			str_list += ch_list;
			continue;
		}str_list += ",";
		str_list += ch_list;
	}
	var = str_list.c_str();
	table_model_->setData(index, var);*/

}

void wid_sub_realtime_moniter::read_dwheel_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__dwheel_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
			tmp.var_id_ = iter.second.id;
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__dwheel_t dwheel_data_;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_, vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id, const void *data) {
		if (!data) {
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)(data);
		if (asio_data_.get_err() < 0) {
			water.sig();
			return;
		}
		ack = *(common_ack*)(data);
		dwheel_data_ = *(var__dwheel_t*)(ack.data.c_str());
		water.sig();
	}));
	if (retval < 0)
	{
		return;
	}
	water.wait();
	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	{
		nsperror << "failed to get dwheel data";
		return;
	}
	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	std::cout << this << " post dwheel command value" << std::endl;
	}
	else{
		std::cout << this << " shared dwheel value" << std::endl;
		dwheel_data_ = *(var__dwheel_t*)(string_data_.c_str());
	}
	index = table_model_->index(0, 1, QModelIndex());
	var = dwheel_data_.max_speed_;
	table_model_->setData(index, var);
	index = table_model_->index(1, 1, QModelIndex());
	var = dwheel_data_.max_acc_;
	table_model_->setData(index, var);
	index = table_model_->index(2, 1, QModelIndex());
	var = dwheel_data_.max_dec_;
	table_model_->setData(index, var);
	index = table_model_->index(3, 1, QModelIndex());

	if (dwheel_data_.control_mode_ >= 0 && dwheel_data_.control_mode_ < 3){
		var = dwheel_ctrlmod[dwheel_data_.control_mode_];
		table_model_->setData(index, var);
	}
	index = table_model_->index(4, 1, QModelIndex());
	var = dwheel_data_.scale_control_;
	table_model_->setData(index, var);
	index = table_model_->index(5, 1, QModelIndex());
	var = dwheel_data_.scale_feedback_;
	table_model_->setData(index, var);
	index = table_model_->index(6, 1, QModelIndex());
	var = dwheel_data_.roll_weight_;
	table_model_->setData(index, var);
	index = table_model_->index(7, 1, QModelIndex());
	var = dwheel_data_.slide_weight_;
	table_model_->setData(index, var);
	index = table_model_->index(8, 1, QModelIndex());
	var = dwheel_data_.enabled_;
	table_model_->setData(index, var);
	index = table_model_->index(9, 1, QModelIndex());
	var = dwheel_data_.actual_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(10, 1, QModelIndex());
	var = dwheel_data_.actual_velocity_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(11, 1, QModelIndex());
	var = dwheel_data_.actual_position_;
	table_model_->setData(index, var);
	index = table_model_->index(12, 1, QModelIndex());
	var = dwheel_data_.actual_position_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(13, 1, QModelIndex());
	var = dwheel_data_.actual_current_;
	table_model_->setData(index, var);
	index = table_model_->index(14, 1, QModelIndex());
	var = dwheel_data_.time_stamp_;
	table_model_->setData(index, var);
	index = table_model_->index(15, 1, QModelIndex());
	var = dwheel_data_.error_code_;
	table_model_->setData(index, var);
	index = table_model_->index(16, 1, QModelIndex());
	var = dwheel_data_.i.enable_;
	table_model_->setData(index, var);
	index = table_model_->index(17, 1, QModelIndex());
	var = dwheel_data_.i.command_velocity_;
	table_model_->setData(index, var);
	index = table_model_->index(18, 1, QModelIndex());
	var = dwheel_data_.i.command_velocity_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(19, 1, QModelIndex());
	var = dwheel_data_.i.command_position_;
	table_model_->setData(index, var);
	index = table_model_->index(20, 1, QModelIndex());
	var = dwheel_data_.i.command_position_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(21, 1, QModelIndex());
	var = dwheel_data_.i.command_current_;
	table_model_->setData(index, var);
	index = table_model_->index(22, 1, QModelIndex());
}

void wid_sub_realtime_moniter::read_swheel_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__swheel_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
			tmp.var_id_ = iter.second.id;
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__swheel_t swheel_data_;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_, vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id, const void *data) {
		if (!data) {
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)(data);
		if (asio_data_.get_err() < 0) {
			water.sig();
			return;
		}
		ack = *(common_ack*)(data);
		swheel_data_ = *(var__swheel_t*)(ack.data.c_str());
		water.sig();
	}));
	if (retval < 0)
	{
		return;
	}
	water.wait();
	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	{
		nsperror << "failed to get swheel data";
		return;
	}
	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	std::cout << this << " post swheel command value" << std::endl;
	}
	else{
		std::cout << this << " shared swheel value" << std::endl;
		swheel_data_ = *(var__swheel_t*)(string_data_.c_str());
	}

	index = table_model_->index(0, 1, QModelIndex());
	var = swheel_data_.min_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(1, 1, QModelIndex());
	var = swheel_data_.max_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(2, 1, QModelIndex());
	var = swheel_data_.zero_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(3, 1, QModelIndex());
	var = swheel_data_.zero_angle_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(4, 1, QModelIndex());
	var = swheel_data_.max_w_;
	table_model_->setData(index, var);
	index = table_model_->index(5, 1, QModelIndex());

	if (swheel_data_.control_mode_ >= 0 && swheel_data_.control_mode_ < 3)
	{
		var = swheel_ctrlmod[swheel_data_.control_mode_];
		table_model_->setData(index, var);
	}

	index = table_model_->index(6, 1, QModelIndex());
	var = swheel_data_.scale_control_;
	table_model_->setData(index, var);
	index = table_model_->index(7, 1, QModelIndex());
	var = swheel_data_.scale_feedback_;
	table_model_->setData(index, var);
	index = table_model_->index(8, 1, QModelIndex());
	var = swheel_data_.control_cp_;
	table_model_->setData(index, var);
	index = table_model_->index(9, 1, QModelIndex());
	var = swheel_data_.control_ci_;
	table_model_->setData(index, var);
	index = table_model_->index(10, 1, QModelIndex());
	var = swheel_data_.control_cd_;
	table_model_->setData(index, var);
	index = table_model_->index(11, 1, QModelIndex());
	var = swheel_data_.enabled_;
	table_model_->setData(index, var);
	index = table_model_->index(12, 1, QModelIndex());
	var = swheel_data_.actual_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(13, 1, QModelIndex());
	var = swheel_data_.actual_angle_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(14, 1, QModelIndex());
	var = swheel_data_.time_stamp_;
	table_model_->setData(index, var);
	index = table_model_->index(15, 1, QModelIndex());
	var = swheel_data_.error_code_;
	table_model_->setData(index, var);
	index = table_model_->index(16, 1, QModelIndex());
	var = swheel_data_.i.enable_;
	table_model_->setData(index, var);
	index = table_model_->index(17, 1, QModelIndex());
	var = swheel_data_.i.command_angle_;
	table_model_->setData(index, var);
	index = table_model_->index(18, 1, QModelIndex());
	var = swheel_data_.i.command_angle_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(19, 1, QModelIndex());
	var = swheel_data_.i.command_rate_;
	table_model_->setData(index, var);
	index = table_model_->index(20, 1, QModelIndex());
	var = swheel_data_.i.command_rate_enc_;
	table_model_->setData(index, var);
	index = table_model_->index(21, 1, QModelIndex());

}

void wid_sub_realtime_moniter::read_sddex_date()
{
	common_t tmp;
	tmp.length_ = sizeof(var__sdd_extra_t);
	tmp.offset_ = 0;
	for (auto&iter : map_combox_index_){
		if (0 == strcmp(table_id_->currentText().toLocal8Bit(), iter.second.desc.c_str()))
			tmp.var_id_ = iter.second.id;
	}
	std::vector<common_t> vct_comm;
	vct_comm.push_back(tmp);
	nsp::os::waitable_handle w_read;
	QModelIndex index;
	QVariant var;
	motion::asio_data asio_data_;
	nsp::os::waitable_handle water;
	var__sdd_extra_t sddex_data_;
	common_ack ack;
	if (other_page_current_id_ != vct_comm[0].var_id_)
	{
		int retval = post_common_read_request_by_id(robot_id_, vct_comm, std::make_shared<motion::asio_partnet>([&](uint32_t robot_id, const void *data) {
		if (!data) {
			water.sig();
			return;
		}
		asio_data_ = *(motion::asio_data*)(data);
		if (asio_data_.get_err() < 0) {
			water.sig();
			return;
		}
		ack = *(common_ack*)(data);
		sddex_data_ = *(var__sdd_extra_t*)(ack.data.c_str());
		water.sig();
	}));
	if (retval < 0)
	{
		return;
	}
	water.wait();
	if (asio_data_.get_err() != nsp::proto::errorno_t::kSuccessful)
	{
		nsperror << "failed to get sddex data";
		return;
	}
	monitor_id_callback_(tmp.var_id_, this, ack.data);//回调给主页面
	std::cout << this << " post sddex command value" << std::endl;
	}
	else{
		std::cout << this << " shared sddex value" << std::endl;
		sddex_data_ = *(var__sdd_extra_t*)(string_data_.c_str());
	}

	index = table_model_->index(0, 1, QModelIndex());
	var = sddex_data_.gauge_;
	table_model_->setData(index, var);
}
