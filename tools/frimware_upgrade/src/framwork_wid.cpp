#include "framwork_wid.h"
#include "task_scheduler.hpp"
#include "log.h"
#include "file_read_handler.h"
#include "network_client_manager.h"
#include "download_btns.h"
#include "upload_btns.h"
#include "ui_upload_btns.h"
#include "ui_download_btns.h"
#include <QtCore\qtimer.h>
#include "net_task_reset.h"
#include <QtWidgets\qcheckbox.h>
#include <direct.h> 
#include <iostream>

framwork_wid::framwork_wid(QWidget *parent) :QMainWindow(parent){
	main_wid_.setupUi(this);
	main_wid_.download_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	main_wid_.upload_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

	//view_model_ = new QStandardItemModel();
	//view_model_->setColumnCount(7);
	//view_model_->setHeaderData(0, Qt::Horizontal, QString::fromLocal8Bit("ip地址"));
	//view_model_->setHeaderData(1, Qt::Horizontal, QString::fromLocal8Bit("型号"));
	//view_model_->setHeaderData(2, Qt::Horizontal, QString::fromLocal8Bit("当前版本"));
	//view_model_->setHeaderData(3, Qt::Horizontal, QString::fromLocal8Bit("CPU型号"));
	//view_model_->setHeaderData(4, Qt::Horizontal, QString::fromLocal8Bit("当前进度"));
	//view_model_->setHeaderData(5, Qt::Horizontal, QString::fromLocal8Bit("当前状态"));
	//view_model_->setHeaderData(6, Qt::Horizontal, QString::fromLocal8Bit(""));

	//main_wid_.download_table_view->setModel(view_model_);
	main_wid_.download_table_view->horizontalHeader()->setDefaultAlignment(Qt::AlignCenter);
	//初始化连接状态为0
	main_wid_.connect->setProperty("connect_status", 0);

	custom_delegate_ = new custom_control_delegate(this);
	//main_wid_.download_table_view->setItemDelegate(custom_delegate_);

	pMapper_ = new QSignalMapper(this);
	if (main_wid_.upload_table_widget->rowCount() != 0){
		for (int i = 0; i < main_wid_.upload_table_widget->rowCount(); i++){
			//add_upload_item();
			QTableWidgetItem*ip_item = new QTableWidgetItem;
			main_wid_.upload_table_widget->setItem(i, 0, ip_item);

			Upload_btns*upload_btn = new Upload_btns;
			upload_btn->ip_item = ip_item;//main_wid_.upload_table_widget->item(i, 0);
			connect(upload_btn, SIGNAL(openClicked(int)), pMapper_, SLOT(map()));//打开
			pMapper_->setMapping(upload_btn, QString::number(i, 10));
			connect(upload_btn, SIGNAL(startClicked(int)), this, SLOT(btn_start_upload_slot(int)));//开始
			connect(upload_btn, SIGNAL(deleteClicked(int)), this, SLOT(btn_delete_slot(int)));//删除
			main_wid_.upload_table_widget->setCellWidget(i, 5, upload_btn);

			/*QPushButton* btn = new QPushButton;
			btn->setText(QString::fromLocal8Bit("上载文件至本地"));
			connect(btn, SIGNAL(clicked()), pMapper_, SLOT(map()));
			pMapper_->setMapping(btn, QString::number(i, 10));
			main_wid_.upload_table_widget->setCellWidget(i, 5, btn);*/
		}
	}

	init_slot();
	start_task_thead();

}

framwork_wid::~framwork_wid(){
	if (net_task_spool_) net_task_spool_->join();
	if (net_task_download_spool_)net_task_download_spool_->join();
	if (net_task_upload_spool_) net_task_upload_spool_->join();
}

void framwork_wid::init_slot(){
	connect(main_wid_.action_xml, SIGNAL(triggered()), this, SLOT(init_xml_file()));
	connect(main_wid_.action_send_down, SIGNAL(triggered()), this, SLOT(action_download()));
	connect(main_wid_.action_get_firmware, SIGNAL(triggered()), this, SLOT(action_upload()));
	connect(main_wid_.action_exit, SIGNAL(triggered()), this, SLOT(exit_application()));

	connect(main_wid_.connect, SIGNAL(clicked()), this, SLOT(connect_vcu()));
	connect(main_wid_.update, SIGNAL(clicked()), this, SLOT(update_vcu_firmware()));
	connect(pMapper_, SIGNAL(mapped(QString)), this, SLOT(upload_file_map(QString)));
	connect(main_wid_.add_upload_btn, SIGNAL(clicked()), this, SLOT(add_upload_item()));
	connect(main_wid_.select_all_checkbox,SIGNAL(clicked()), this, SLOT(select_all_check()));

	//connect(view_model_, SIGNAL(dataChanged(const QModelIndex &, const QModelIndex &, const QVector<int> &)),
	//	this, SLOT(btn_slot_clicked(const QModelIndex &, const QModelIndex &, const QVector<int> &)));
}

void framwork_wid::init_xml_file(){
	QString fileName;
	QFileDialog* fd = new QFileDialog(this);//创建对话框
	fd->resize(240, 320);    //设置显示的大小
	QStringList filters;
	filters << "xmlfile(*.xml)";
	fd->setNameFilters(filters); //设置文件过滤器
	fd->setViewMode(QFileDialog::List);  //设置浏览模式，有列表（list） 模式和 详细信息（detail）两种方式
	if (fd->exec() == QDialog::Accepted)   //如果成功的执行
	{
		QStringList fileNameList;
		fileNameList = fd->selectedFiles();
		if (fileNameList.empty()){
			fd->close();
			return;
		}
		fileName = fileNameList[0];
		//设置标题栏
		this->setWindowTitle(fileName);
		//加载xml文件
		if (config_info_.read_xml_file((std::string)fileName.toLocal8Bit(), firm_info_) < 0){
			//打开失败，则界面不可操作
			main_wid_.toolBar->setEnabled(false);
			main_wid_.stackedWidget->setEnabled(false);
			QMessageBox::warning(this, fileName,
				tr("failed to open xml file,please check it up."),
				QMessageBox::Ok);
			return;
		}
		//加载文件至内存中
		if (nsp::toolkit::singleton<file_read_handler>::instance()->open_file(firm_info_.firmware_path_) < 0){
			QMessageBox::warning(this, QString::fromLocal8Bit(firm_info_.firmware_path_.c_str()), 
				tr("failed to load bin file."), QMessageBox::Ok);
			return;
		}

		//所有加载均有效情况下打开操作
		main_wid_.toolBar->setEnabled(true);
		main_wid_.stackedWidget->setEnabled(true);
		main_wid_.download_page->setEnabled(true);
		main_wid_.upload_page->setEnabled(true);
		main_wid_.update->setEnabled(false);
		main_wid_.target_cpu_value->setText(QString::fromLocal8Bit(firm_info_.firmware_cpu_type_.c_str()));
		main_wid_.target_moule_value->setText(QString::fromLocal8Bit(firm_info_.modules_type_.c_str()));
		main_wid_.target_vesion_value->setText(QString::fromLocal8Bit(firm_info_.firmware_version_.c_str()));
		main_wid_.bin_file_value->setText(QString::fromLocal8Bit(firm_info_.firmware_name_.c_str()));
	}
	fd->close();
}

void framwork_wid::action_download(){
	main_wid_.stackedWidget->setCurrentIndex(0);
}

void framwork_wid::action_upload(){
	main_wid_.stackedWidget->setCurrentIndex(1);
}

void framwork_wid::exit_application(){
	QApplication::quit();
}

void framwork_wid::connect_vcu(){
	std::string local_ip = main_wid_.line_ip_local->text().toLocal8Bit();
	std::string ip_start = main_wid_.line_ip_begin->text().toLocal8Bit();
	std::string ip_end = main_wid_.line_ip_end->text().toLocal8Bit();
	std::vector < std::string > vct_ip;
	map_ip_index_.clear();
	if (ip_separate(ip_start, ip_end, vct_ip) < 0){
		return;
	}
	int port = main_wid_.line_port->text().toInt();
	std::string ip_port = "0.0.0.0:" + main_wid_.line_port->text().toLocal8Bit();
	if (main_wid_.connect->property("connect_status") == 0){
		main_wid_.connect->setText(QStringLiteral("断开"));
		main_wid_.connect->setProperty("connect_status", 1);
		main_wid_.update->setEnabled(true);
		nsp::toolkit::singleton<network_server_manager>::instance()->listen(ip_port);
	}
	else{
		main_wid_.connect->setText(QStringLiteral("连接"));
		main_wid_.connect->setProperty("connect_status", 0);
		main_wid_.update->setEnabled(false);
		nsp::toolkit::singleton<network_server_manager>::instance()->close_udp();
		nsp::toolkit::singleton<network_client_manager>::instance()->clean_session();
		return;
	}
	main_wid_.download_table_view->setRowCount(0);
	main_wid_.download_table_view->setRowCount(vct_ip.size());
	for (int row = 0; row < main_wid_.download_table_view->rowCount(); ++row) {
		for (int column = 0; column < main_wid_.download_table_view->columnCount(); ++column) {
			if (column == main_wid_.download_table_view->columnCount() - 1){
				QCheckBox*box = new QCheckBox;
				main_wid_.download_table_view->setCellWidget(row, column, box);
			}
			QTableWidgetItem* item = new QTableWidgetItem;
			main_wid_.download_table_view->setItem(row, column, item);
		
			if (column == main_wid_.download_table_view->columnCount() - 2){
				Download_btns*download_btn = new Download_btns;
				download_btn->row_ = row;
				connect(download_btn, SIGNAL(startClicked(int)), this, SLOT(btn_start_slot(int)));//开始
				connect(download_btn, SIGNAL(restartClicked(int)), this, SLOT(btn_restart_slot(int)));//重新获取
				connect(download_btn, SIGNAL(resetClicked(int)), this, SLOT(btn_reset_slot(int)));//重启
				main_wid_.download_table_view->setCellWidget(row, column, download_btn);
			}
		}
	}

	//view_model_->removeRows(0,view_model_->rowCount());
	for (size_t i = 0; i < vct_ip.size(); i++){
		main_wid_.download_table_view->setItem(i, 0, new QTableWidgetItem(QString::fromLocal8Bit(vct_ip[i].c_str())));
		//view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_ip[i].c_str())));
		{
			std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
			map_ip_index_.insert(std::make_pair(vct_ip[i], i));
		}
		get_vcu_info_task(local_ip, vct_ip[i], port);
	}
}

void framwork_wid::btn_start_slot(int row)
{
	if (firm_info_.firmware_version_control_ == 1){
		//要求版本控制，通知用户是否进行选择操作
		if (QMessageBox::No == QMessageBox::warning(this, QStringLiteral("警告："), QStringLiteral("当前固件文件有版本控制属性，是否继续升级？")
			, QMessageBox::Yes | QMessageBox::No)){
			return;
		}
	}
	std::string ip_str = main_wid_.download_table_view->item(row, 0)->text().toLocal8Bit();
	update_vcu_bin_task(ip_str);
}
void framwork_wid::btn_restart_slot(int row)
{
	std::string local_ip = main_wid_.line_ip_local->text().toLocal8Bit();
	std::string ip_str = main_wid_.download_table_view->item(row, 0)->text().toLocal8Bit();
	int port = main_wid_.line_port->text().toInt();
	get_vcu_info_task(local_ip, ip_str, port);
}
void framwork_wid::btn_reset_slot(int row)
{
	nsp::toolkit::singleton<net_task_reset>::instance()->regiset_callback(std::bind(&framwork_wid::update_net_recv_download, this, std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4));
	nsp::toolkit::singleton<net_task_reset>::instance()->regiset_get_vcu_callback(std::bind(&framwork_wid::update_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
		std::placeholders::_3, std::placeholders::_4));
	std::string ip = main_wid_.download_table_view->item(row, 0)->text().toLocal8Bit();
	if (nsp::toolkit::singleton<net_task_reset>::instance()->net_reset(ip) == 0){
		reset_wait_.wait(firm_info_.reset_wait_time_);
		nsp::toolkit::singleton<net_task_reset>::instance()->get_vcu_type_version_request((FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, ip, firm_info_.firmware_version_control_);
	}

}
void framwork_wid::btn_start_upload_slot(int row)
{
	std::string local_ip = main_wid_.line_ip_local->text().toLocal8Bit();
	QTableWidgetItem* tab_item = main_wid_.upload_table_widget->item(row, 0);
	if (tab_item == NULL){
		return;
	}
	QTableWidgetItem* port_item = main_wid_.upload_table_widget->item(row, 1);
	if (port_item == NULL){
		return;
	}
	std::string ip_str = tab_item->text().toLocal8Bit();
	uint32_t port = port_item->text().toUInt();
	
	auto iter = map_ip_path_.find(ip_str);
	if (iter == map_ip_path_.end()){
		QMessageBox::warning(this, QStringLiteral("警告："), QStringLiteral("没有添加文件！"), QMessageBox::Ok);
		return;
	}

	//创建任务，放进线程池中
	std::shared_ptr<net_task_upload> task = nullptr;
	try{
		task = std::make_shared<net_task_upload>(local_ip, ip_str, port, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, firm_info_.firmware_length_, iter->second);
	}
	catch (...){
		nsperror << "new net_task_upload make shared failed!";
		return;
	}
	if (task){
		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_upload, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
	}
	if (!net_task_upload_spool_){
		return;
	}
	net_task_upload_spool_->post(task);	

	Upload_btns *upload = (Upload_btns*)main_wid_.upload_table_widget->cellWidget(row, 5);
	upload->ui.delete_->setEnabled(false);
}

void framwork_wid::btn_delete_slot(int row)
{
	//如果map里存了该行ip需删除
	QTableWidgetItem* tab_item = main_wid_.upload_table_widget->item(row, 0);
	std::string ip_str = tab_item->text().toLocal8Bit();
	auto iter = map_ip_path_.find(ip_str);
	if (iter != map_ip_path_.end()){
		map_ip_path_.erase(iter);
	}
	main_wid_.upload_table_widget->removeRow(row);
}
void framwork_wid::update_vcu_firmware(){
	int port = main_wid_.line_port->text().toInt();
	if (firm_info_.firmware_version_control_ == 1){
		//要求版本控制，通知用户是否进行选择操作
		if (QMessageBox::No == QMessageBox::warning(this, QStringLiteral("警告："), QStringLiteral("当前固件文件有版本控制属性，是否继续升级？")
			, QMessageBox::Yes | QMessageBox::No)){
			return;
		}
	}
	
	for (size_t i = 0; i < main_wid_.download_table_view->rowCount(); i++){
		//读取状态item是否允许升级
		QTableWidgetItem* mode_item = main_wid_.download_table_view->item(i, 5);	
		QCheckBox *checkBox = (QCheckBox*)main_wid_.download_table_view->cellWidget(i, main_wid_.download_table_view->columnCount() - 1);
		if (mode_item && !(mode_item->data(Qt::UserRole).toBool()))continue;
		if (checkBox->checkState() == Qt::Unchecked)continue;
		std::string ip = main_wid_.download_table_view->item(i, 0)->text().toLocal8Bit();
		update_vcu_bin_task(ip);
	}
}

void framwork_wid::btn_slot_clicked(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
	if (topLeft.column() != 6 && roles.size() == 0){
		return;
	}
	std::string local_ip = main_wid_.line_ip_local->text().toLocal8Bit();
	std::string ip_str;// = main_wid_.download_table_view->item(row, 0)->text.toLocal8Bit();/*= view_model_->item(topLeft.row(), 0)->text().toLocal8Bit()*/
	int port = main_wid_.line_port->text().toInt();
	//根据按钮属性，分别进行不同的操作
	switch (roles[0])
	{
	case Btn_ReGet:
		get_vcu_info_task(local_ip, ip_str, port);
		break;
	case Btn_Retry:
		update_vcu_bin_task(ip_str);
		break;
	case Btn_Degrade:
		update_vcu_bin_task(ip_str);
		break;
	default:
		break;
	}
}

//创建任务，放进线程池中待执行
void framwork_wid::get_upload_vcu_info_task(const std::string& lcoal_ip, const std::string& ip, const int port){
	std::shared_ptr<net_task> task = nullptr;
	try{
		task = std::make_shared<net_task>(lcoal_ip, ip, port, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_version_control_);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}
	if (task){
		task->regiset_callback(std::bind(&framwork_wid::update_upload_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
	}
	if (!net_task_spool_){
		return;
	}
	net_task_spool_->post(task);
}

void framwork_wid::get_vcu_info_task(const std::string& lcoal_ip, const std::string& ip, const int port){
	std::shared_ptr<net_task> task = nullptr;
	try{
		task = std::make_shared<net_task>(lcoal_ip, ip, port, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_version_control_);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}
	if (task){
		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
	}	
	if (!net_task_spool_){
		return;
	}
	net_task_spool_->post(task);
}

//创建任务，放进线程池中
void framwork_wid::update_vcu_bin_task(const std::string& ip){
	std::shared_ptr<net_task_download> task = nullptr;
	try{
		task = std::make_shared<net_task_download>(ip, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, firm_info_.reset_wait_time_, firm_info_.block_write_delay_, firm_info_.firmware_version_control_);
	}
	catch (...){
		nsperror << "new net_task_download make shared failed!";
		return;
	}
	if (task){
		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_download, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
		task->regiset_get_vcu_callback(std::bind(&framwork_wid::update_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
	}
	if (!net_task_download_spool_){
		return;
	}
	net_task_download_spool_->post(task);
}

void framwork_wid::upload_file_map(QString index){
	std::string local_ip = main_wid_.line_ip_local->text().toLocal8Bit();
	int tab_index = index.toInt();
	QTableWidgetItem* tab_item = main_wid_.upload_table_widget->item(tab_index, 0);
	if (tab_item == NULL){
		return;
	}
	QTableWidgetItem* port_item = main_wid_.upload_table_widget->item(tab_index, 1);
	if (port_item == NULL){
		return;
	}
	uint32_t port = port_item->text().toUInt();
	QString fileName = QFileDialog::getSaveFileName(this,
		QStringLiteral("保存bin文件"),
		"",
		tr("Config Files(*.bin)"));
	if (!fileName.isNull()){		//文件选择后再去获取 型号和版本

		std::string ip_str = tab_item->text().toLocal8Bit();
		std::string file_name_str = fileName.toLocal8Bit();
		auto iter = map_ip_path_.find(ip_str);
		if (iter != map_ip_path_.end()){
			QMessageBox::warning(this, QStringLiteral("警告："), QStringLiteral("存在相同 ip！"), QMessageBox::Ok);
			main_wid_.upload_table_widget->item(tab_index, 0)->setData(Qt::DisplayRole, QStringLiteral(""));
			main_wid_.upload_table_widget->item(tab_index, 1)->setData(Qt::DisplayRole, QStringLiteral(""));
			return;
		}
		map_ip_path_.insert(std::make_pair(ip_str, file_name_str));
		get_upload_vcu_info_task(local_ip, ip_str, port);
	}

	//if (!fileName.isNull())
	//{
	//	//创建任务，放进线程池中
	//	std::shared_ptr<net_task_upload> task = nullptr;
	//	try{
	//		std::string ip_str = tab_item->text().toLocal8Bit();
	//		std::string file_name_str = fileName.toLocal8Bit();
	//		task = std::make_shared<net_task_upload>(local_ip, ip_str, port, (FIRMWARE_SOFTWARE_TYPE)firm_info_.firmware_type, firm_info_.firmware_length_, file_name_str);
	//	}
	//	catch (...){
	//		nsperror << "new net_task_upload make shared failed!";
	//		return;
	//	}
	//	if (task){
	//		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_upload, this, std::placeholders::_1, std::placeholders::_2,
	//			std::placeholders::_3, std::placeholders::_4));
	//	}
	//	if (!net_task_upload_spool_){
	//		return;
	//	}
	//	net_task_upload_spool_->post(task);
	//}
}

void framwork_wid::download_operate_map(QString index){
	int tab_index = index.toInt();
}

void framwork_wid::select_all_check()
{
	for (int row = 0; row < main_wid_.download_table_view->rowCount(); ++row){
		QCheckBox *checkBox=(QCheckBox*)main_wid_.download_table_view->cellWidget(row, main_wid_.download_table_view->columnCount()-1);
		main_wid_.select_all_checkbox->checkState() == Qt::Checked ? checkBox->setCheckState(Qt::Checked) : checkBox->setCheckState(Qt::Unchecked);
		
	}
}

void framwork_wid::add_upload_item(){
	int index = main_wid_.upload_table_widget->rowCount();
	main_wid_.upload_table_widget->setRowCount(main_wid_.upload_table_widget->rowCount() + 1);

	QTableWidgetItem*ip_item = new QTableWidgetItem;
	main_wid_.upload_table_widget->setItem(index,0,ip_item);

	Upload_btns*upload_btn = new Upload_btns;
	upload_btn->ip_item = ip_item;//main_wid_.upload_table_widget->item(index,0);
	connect(upload_btn, SIGNAL(openClicked(int)), pMapper_, SLOT(map()));//打开
	pMapper_->setMapping(upload_btn, QString::number(index, 10));
	connect(upload_btn, SIGNAL(startClicked(int)), this, SLOT(btn_start_upload_slot(int)));//开始
	connect(upload_btn, SIGNAL(deleteClicked(int)), this, SLOT(btn_delete_slot(int)));//删除
	main_wid_.upload_table_widget->setCellWidget(index, 5, upload_btn);
	//QPushButton* btn = new QPushButton;
	//btn->setText(QString::fromLocal8Bit("上载文件至本地"));
	//connect(btn, SIGNAL(clicked()), pMapper_, SLOT(map()));
	//pMapper_->setMapping(btn, QString::number(index, 10));
	//main_wid_.upload_table_widget->setCellWidget(index, 5, btn);
}

int framwork_wid::ip_separate(const std::string&ip_start, const std::string&ip_end, 
	std::vector<std::string>& vct_str){
	int ip_start_int = atoi(ip_start.substr(ip_start.find_last_of('.') + 1).c_str());
	int ip_end_int = atoi(ip_end.substr(ip_start.find_last_of('.') + 1).c_str());
	if (ip_start_int > ip_end_int){
		return -1;
	}
	std::string ip_prefix = ip_start.substr(0, ip_start.find_last_of('.'));
	for (int i = ip_start_int; i <= ip_end_int; i++){
		char tmp[16];
		sprintf_s(tmp, sizeof(tmp), ".%d", i);
		vct_str.push_back(ip_prefix + tmp);
	}
	return 0;
}

int framwork_wid::start_task_thead(){
	if (!net_task_spool_ && !net_task_download_spool_ && !net_task_upload_spool_){
		try{
			net_task_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<net_task>>(NET_WINDOW_SIZE);
			net_task_download_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<net_task_download>>(NET_WINDOW_SIZE);
			net_task_upload_spool_ = std::make_shared<nsp::toolkit::task_thread_pool<net_task_upload>>(NET_WINDOW_SIZE);
		}
		catch (...){
			nsperror << "start_task_thead failed!";
			return -1;
		}
	}
	return 0;
}

void framwork_wid::update_upload_net_recv_data(const std::string& ip, const int data_type, const std::string& data, const nsp::proto::errorno_t status)
{
	for (size_t i = 0; i < main_wid_.upload_table_widget->rowCount(); i++){

		QTableWidgetItem*item = main_wid_.upload_table_widget->item(i, 0);
		
		if (item && ((std::string)item->text().toLocal8Bit() == ip)){
			QTableWidgetItem* item = 0;
			if (FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE == data_type){
				item = new QTableWidgetItem;
				main_wid_.upload_table_widget->setItem(i, 2, item);
				//item = main_wid_.upload_table_widget->item(i, 2);
			}
			else if (FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION == data_type){
				item = new QTableWidgetItem;
				main_wid_.upload_table_widget->setItem(i, 3, item);
				//item = main_wid_.upload_table_widget->item(i, 3);
			}
			else if (FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION == data_type){
				
			}
			if (item)
			{
				item->setText(QString::fromLocal8Bit(data.c_str()));
			}

		}
	}
}

void framwork_wid::update_net_recv_data(const std::string& ip, const int data_type, const std::string& data, const nsp::proto::errorno_t status){
	//std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
	//ip_ = ip;
	//data_type_ = data_type;
	//data_ = data;
	//status_ = status;
	//QMetaObject::invokeMethod(this, "update_recv_data");
	//QTimer::singleShot(0, this, SLOT(update_recv_data()));

	auto iter = map_ip_index_.find(ip);
	if (iter == map_ip_index_.end()){
		return;
	}
	//QModelIndex index;
	QTableWidgetItem* item = 0;
	if (FIRMWARE_SUB_OPERATE_CODE_GET_VCU_TYPE == data_type){
		item = main_wid_.download_table_view->item(iter->second, 1);
		if (status == nsp::proto::kSuccessful){
			//比较型号
			if (firm_info_.modules_type_ != data.c_str()){
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("型号不一致"));
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::UserRole, false);
			}
			else{
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::UserRole, true);
			}
		}
		else{
			main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("无法连接"));
			main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::UserRole, false);
			////main_wid_.download_table_view->item(iter->second, 6)->setData(Qt::DisplayRole, QStringLiteral("重新获取"));
		}
	}
	else if (FIRMWARE_SUB_OPERATE_CODE_GET_VCU_VERSION == data_type){
		item = main_wid_.download_table_view->item(iter->second, 2);
		if (status == nsp::proto::kSuccessful){
			//做版本比较操作
			if (data.c_str() > firm_info_.firmware_version_){
				//vcu更高版本
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("版本更高"));
				////main_wid_.download_table_view->item(iter->second, 6)->setData(Qt::DisplayRole, QStringLiteral("降级"));
			}
		}
	}
	else if (FIRMWARE_SUB_OPERATE_CODE_GET_CPU_VERSION == data_type){
		item = main_wid_.download_table_view->item(iter->second, 3);
		if (status == nsp::proto::kSuccessful){
			if (firm_info_.firmware_cpu_type_ != data.c_str()){
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("CPU版本不一致"));
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::UserRole, false);
			}
			else{
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::UserRole, true);
			}
		}
	}
	else if (FIRMWARE_COMPLETE_GET_VCU_VERSION == data_type){
		item = main_wid_.download_table_view->item(iter->second, 2);
		if (status == nsp::proto::kSuccessful){
			index_value_ = iter->second;
			if (firm_info_.firmware_version_control_ == 0){
				reset_status_ = QStringLiteral("重启完成");
				QTimer::singleShot(0, this, SLOT(update_reset_final_lstatus()));
				//main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("重启完成"));
			}
			//做版本比较操作
			else if (firm_info_.firmware_version_control_ == 1){
				if (data.c_str() == firm_info_.firmware_version_){
					//版本相同即成功
					reset_status_ = QStringLiteral("重启完成");
					QTimer::singleShot(0, this, SLOT(update_reset_final_lstatus()));
					//main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("重启完成"));		
				}
				else{
					reset_status_ = QStringLiteral("重启失败");
					QTimer::singleShot(0, this, SLOT(update_reset_final_lstatus()));
					//main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("失败"));
					////main_wid_.download_table_view->item(iter->second, 6)->setData(Qt::DisplayRole, QStringLiteral("重新升级"));
				}
			}
			
		}
	}
	
	//if (!index.isValid()){
	//	return;
	//}
	//QVariant var = QString::fromLocal8Bit(data.c_str());
	//view_model_->setData(index, var);
	//main_wid_.download_table_view->reset();
	if (item)
		item->setText(QString::fromLocal8Bit(data.c_str()));
		//item->setData(Qt::DisplayRole, QString::fromLocal8Bit(data.c_str()));
}

void framwork_wid::update_net_recv_download(const std::string& ip, const int type, const int radi, const nsp::proto::errorno_t status){
	std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
	auto iter = map_ip_index_.find(ip);
	if (iter == map_ip_index_.end()){
		return;
	}
	switch (type)
	{
	case FIRMWARE_SUB_OPERATE_CODE_READ_NORAML_FILE:
		update_download_compare_status(iter->second, radi, status);
		break;
	case FIRMWARE_MAIN_OPERATE_CODE_RW_FIRMWARE:
		update_download_progress_status(iter->second, radi, status);
		break;
	case FIRMWARE_RESET_OPERATE_CODE_CALLBACK:
		update_download_reset_status(iter->second, radi, status);
		break;
	default:
		break;
	}
	main_wid_.download_table_view->update();
}

void framwork_wid::update_download_compare_status(const int index_value, const int radi, const nsp::proto::errorno_t status){
	if (status == nsp::proto::errorno_t::kUnsuccessful){
		
		main_wid_.download_table_view->item(index_value, 5)->setData(Qt::DisplayRole, QStringLiteral("核对失败"));
		////main_wid_.download_table_view->item(index_value, 6)->setData(Qt::DisplayRole, QStringLiteral("重试"));
	/*	QModelIndex index = view_model_->index(index_value, 5, QModelIndex());
		QVariant var = QStringLiteral("核对失败");
		view_model_->setData(index, var);
		view_model_->setData(view_model_->index(index_value, 6, QModelIndex()), QVariant(QStringLiteral("重试")));*/
	}
}
void framwork_wid::update_radi()
{
	QTableWidgetItem* item = main_wid_.download_table_view->item(index_value_, 4);
	item->setData(Qt::DisplayRole, radi_);
	item=main_wid_.download_table_view->item(index_value_, 5);
	item->setData(Qt::DisplayRole, radi_ != 100 ? QStringLiteral("正在升级") : QStringLiteral("下载完成,正在核对..."));
}
void framwork_wid::update_download_progress_status(const int index_value, const int radi, const nsp::proto::errorno_t status){
	if (status == nsp::proto::errorno_t::kSuccessful){
		//更新进度条
		//QTableWidgetItem* item = main_wid_.download_table_view->item(index_value, 4);
		//item->setData(Qt::DisplayRole, radi);
		index_value_ = index_value;
		radi_ = radi;
		QTimer::singleShot(0,this,SLOT(update_radi()));

		//更新状态条
		//item = main_wid_.download_table_view->item(index_value, 5);
		//item->setData(Qt::DisplayRole, radi != 100 ? QStringLiteral("正在升级") : QStringLiteral("下载完成,正在核对..."));

	}
	else{
		QTableWidgetItem* item = main_wid_.download_table_view->item(index_value, 5);
		item->setData(Qt::DisplayRole, QStringLiteral("升级失败"));
		main_wid_.download_table_view->item(index_value, 6)->setData(Qt::DisplayRole, QStringLiteral("重试"));
	}
}
void framwork_wid::update_reset_final_lstatus()
{
	QTableWidgetItem* item = main_wid_.download_table_view->item(index_value_, 5);
	item->setData(Qt::DisplayRole, reset_status_);
}
void framwork_wid::update_reset_status()
{
	QTableWidgetItem* item = main_wid_.download_table_view->item(index_value_, 5);
	item->setData(Qt::DisplayRole, var_);
}
void framwork_wid::update_download_reset_status(const int index_value, const int radi, const nsp::proto::errorno_t status){
	index_value_ = index_value;
	//QTableWidgetItem* item = main_wid_.download_table_view->item(index_value, 5);
	switch (status)
	{
	case nsp::proto::errorno_t::kSuccessful:
		var_ = QStringLiteral("正在重启");
		break;
	case nsp::proto::errorno_t::kRequestTimeout:
		var_ = QStringLiteral("等待重启超时");
		break;
	case nsp::proto::errorno_t::kUnsuccessful:
		var_ = QStringLiteral("重启失败");
		break;
	default:
		break;
	}
	//QTableWidgetItem*item=main_wid_.download_table_view->item(index_value, 5);
	//item->setData(Qt::DisplayRole, var_);
	QTimer::singleShot(0, this, SLOT(update_reset_status()));
	qApp->processEvents();
}

void framwork_wid::update_net_recv_upload(const std::string& ip, const int type, const int radi, const nsp::proto::errorno_t status){
	std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
	for (size_t i = 0; i < main_wid_.upload_table_widget->rowCount(); i++){
		QTableWidgetItem* item = main_wid_.upload_table_widget->item(i, 0);
		if (item && ((std::string)item->text().toLocal8Bit() == ip)){
			QString show_info;
			if (status == nsp::proto::kUnsuccessful){
				show_info = QStringLiteral("获取失败");
			}
			else if (status == nsp::proto::kSuccessful){
				show_info = QStringLiteral("进度：");
				show_info += QVariant(radi).toString();
				show_info += " %";
			}
			QTableWidgetItem* child_item = main_wid_.upload_table_widget->item(i, 4);
			if (child_item){
				child_item->setText(show_info);
			}
			else{
				QTableWidgetItem* new_item = new QTableWidgetItem(show_info);
				main_wid_.upload_table_widget->setItem(i, 4, new_item);
			}
			if (radi == 100){
				for (size_t i = 0; i < main_wid_.upload_table_widget->rowCount(); i++){
					QTableWidgetItem*item = main_wid_.upload_table_widget->item(i, 0);
					if (item && ((std::string)item->text().toLocal8Bit() == ip)){
						QTimer::singleShot(0, this, SLOT(set_delete_btn_status()));
						index_delete_ = i;
					}

				}

			}
			return;
		}
	}
}

void framwork_wid::set_delete_btn_status()
{
	Upload_btns *upload = (Upload_btns*)main_wid_.upload_table_widget->cellWidget(index_delete_, 5);
	upload->ui.delete_->setEnabled(true);
}