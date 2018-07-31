#include "framwork_wid.h"
#include "task_scheduler.hpp"
#include "log.h"
#include "file_read_handler.h"
#include "network_client_manager.h"
#include "download_btns.h"
#include "upload_btns.h"
#include "ui_upload_btns.h"
#include "ui_download_btns.h"
#include "comm_impl.hpp"
#include "basic_def.h"
#include <QtCore\qtimer.h>
#include "net_task_reset.h"
#include "watcherdialog.hpp"
#include <QtWidgets\qcheckbox.h>
#include <direct.h> 
#include <iostream>

namespace
{
	static const QString NODE_ID_MATCH = "\\d+-\\d+";
}

framwork_wid::framwork_wid(QWidget *parent) 
:QMainWindow(parent),
m_nodeIDValidator{ QSharedPointer<QRegExpValidator>::create(QRegExp(NODE_ID_MATCH)) }
{
	main_wid_.setupUi(this);

	if (m_nodeIDValidator.isNull())
	{
		return;
	}

	main_wid_.edit_node_id->setValidator(m_nodeIDValidator.data());
	main_wid_.download_table_view->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	main_wid_.upload_table_widget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	type_device_ = kdevice_vcu_default;
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
	resize(1024, 768);
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

	connect(main_wid_.combo_serial_id, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_combox_serial_id_changed(const QString &)));
	connect(main_wid_.combo_style, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(on_combox_style_changed(const QString &)));
	connect(this, SIGNAL(updatestatussignal(QString, int, QString, int)),
		this, SLOT(update_status(QString, int, QString, int)));
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
		main_wid_.combo_style->setCurrentIndex(0);

		if (main_wid_.combo_style->currentText().toLower().compare("vcu") == 0)
		{
			main_wid_.update->setEnabled(true);
			main_wid_.connect->setEnabled(true);
			main_wid_.combo_serial_id->setEnabled(true);
		}
		else
		{
			main_wid_.update->setEnabled(false);
			main_wid_.connect->setEnabled(false);
			main_wid_.combo_serial_id->setEnabled(false);
		}

		//main_wid_.target_cpu_value->setText(QString::fromLocal8Bit(firm_info_.firmware_cpu_type_.c_str()));
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
	std::string ip_start = main_wid_.line_ip_begin->text().toLocal8Bit();
	std::string ip_end = main_wid_.line_ip_end->text().toLocal8Bit();
	std::vector < std::string > vct_ip;
	map_ip_index_.clear();

	if (ip_separate(ip_start, ip_end, vct_ip) < 0){
		return;
	}

	if (main_wid_.connect->property("connect_status") == 0){
		main_wid_.connect->setText(QStringLiteral("断开"));
		main_wid_.connect->setProperty("connect_status", 1);
		main_wid_.update->setEnabled(true);
		main_wid_.line_ip_begin->setEnabled(false);
		main_wid_.line_ip_end->setEnabled(false);
		main_wid_.line_port->setEnabled(false);
		main_wid_.edit_node_id->setEnabled(false);
		main_wid_.combo_serial_id->setEnabled(false);
	}
	else{
		main_wid_.connect->setText(QStringLiteral("连接"));
		main_wid_.connect->setProperty("connect_status", 0);
		main_wid_.update->setEnabled(false);
		main_wid_.line_ip_begin->setEnabled(true);
		main_wid_.line_ip_end->setEnabled(true);
		main_wid_.line_port->setEnabled(true);
		main_wid_.edit_node_id->setEnabled(true);
		main_wid_.combo_serial_id->setEnabled(true);

		nsp::toolkit::singleton<network_client_manager>::instance()->clean_session();
		main_wid_.download_table_view->setRowCount(0);
		return;
	}

	bool ok = false;
	int startID = 0, endID = 0;
	const QString &nodeIDStr = main_wid_.edit_node_id->text();

	if (!nodeIDStr.contains(QRegExp(NODE_ID_MATCH)))
	{
		startID = endID = nodeIDStr.toInt(&ok);

		if (!ok)
		{
			return;
		}
	}
	else
	{
		QString startIDStr;

		if (!getStringBySeparator(startIDStr, nodeIDStr, static_cast<int>(NodeIDField::NIF_StartNodeID), LINE_SEPARATOR))
		{
			return;
		}

		startID = startIDStr.toInt(&ok);

		if (!ok)
		{
			return;
		}

		QString endIDStr;

		if (!getStringBySeparator(endIDStr, nodeIDStr, static_cast<int>(NodeIDField::NIF_EndNodeID), LINE_SEPARATOR))
		{
			return;
		}

		endID = endIDStr.toInt(&ok);

		if (!ok)
		{
			return;
		}
	}

	int nodeIDCount = endID - startID + 1;
	int vct_ip_size = vct_ip.size();
	int rowCount = vct_ip_size * nodeIDCount;
	main_wid_.select_all_checkbox->setCheckState(Qt::Unchecked);
	main_wid_.download_table_view->clearContents();
	main_wid_.download_table_view->setRowCount(rowCount);

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

	QStringList ipNodeIDStrList;

	for (int i = 0; i < vct_ip_size; ++i)
	{
		for (int j = startID; j <= endID; ++j)
		{
			const QString &ipNodeIDStr = QString("%1%2%3").arg(QString::fromStdString(vct_ip[i])).arg(LINE_SEPARATOR).arg(j);
			ipNodeIDStrList << ipNodeIDStr;
		}
	}

	int ipNodeIDStrListSize = ipNodeIDStrList.size();
	Q_ASSERT(ipNodeIDStrListSize == rowCount);

	if (ipNodeIDStrListSize != rowCount)
	{
		return;
	}

	std::string local_ip = NET_ADDRESS;
	int port = main_wid_.line_port->text().toInt();
	//view_model_->removeRows(0,view_model_->rowCount());
	for (size_t i = 0; i < rowCount; i++){
		const QString &ipNodeIDStr = ipNodeIDStrList[i];
		main_wid_.download_table_view->setItem(i, 0, new QTableWidgetItem(ipNodeIDStr));
		//view_model_->setItem(i, 0, new QStandardItem(QString::fromLocal8Bit(vct_ip[i].c_str())));
		{
			std::lock_guard<decltype(lock_mutex_)> lock(lock_mutex_);
			map_ip_index_.insert(std::make_pair(ipNodeIDStr.toStdString(), i));
		}

		start_data_forward(local_ip, ipNodeIDStr.toStdString(), port);
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
	
	const QString &ipNodeIDStr = main_wid_.download_table_view->item(row, 0)->text();
	QString nodeIDStr;

	if (!getStringBySeparator(nodeIDStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
	{
		return;
	}

	bool ok = false;
	uint8_t node_id = nodeIDStr.toUInt(&ok);

	if (!ok)
	{
		return;
	}

	update_vcu_bin_task(row, ipNodeIDStr.toStdString(), node_id);
}
void framwork_wid::btn_restart_slot(int row)
{
	const QString &ipNodeIDStr = main_wid_.download_table_view->item(row, 0)->text();
	QString ipStr;

	if (!getStringBySeparator(ipStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Ip), LINE_SEPARATOR))
	{
		return;
	}

	QString nodeIDStr;

	if (!getStringBySeparator(nodeIDStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
	{
		return;
	}

	bool ok = false;
	uint8_t node_id = nodeIDStr.toUInt(&ok);

	if (!ok)
	{
		return;
	}

	std::string local_ip = NET_ADDRESS;
	int port = main_wid_.line_port->text().toInt(&ok);

	if (!ok)
	{
		return;
	}

	uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
	get_vcu_info_task(local_ip, ipStr.toStdString(), node_id, serial_id, port);
}
void framwork_wid::btn_reset_slot(int row)
{
	const QString &ipNodeIDStr = main_wid_.download_table_view->item(row, 0)->text();
	QString ipStr;

	if (!getStringBySeparator(ipStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Ip), LINE_SEPARATOR))
	{
		return;
	}

	QString nodeIDStr;

	if (!getStringBySeparator(nodeIDStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
	{
		return;
	}

	bool ok = false;
	uint8_t node_id = nodeIDStr.toUInt(&ok);

	if (!ok)
	{
		return;
	}

	int port = main_wid_.line_port->text().toInt();
	std::string local_ip = NET_ADDRESS;
	uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
	get_vcu_cpu_task(local_ip, ipStr.toStdString(), port, serial_id, node_id);
}
void framwork_wid::btn_start_upload_slot(int row)
{
	std::string local_ip = NET_ADDRESS;
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
		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
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
		QCheckBox *checkBox = (QCheckBox*)main_wid_.download_table_view->cellWidget(i, main_wid_.download_table_view->columnCount() - 1);

		if (nullptr == checkBox)
		{
			continue;
		}

		if (checkBox->checkState() != Qt::Checked)
		{
			continue;
		}

		const QString &ipNodeIDStr = main_wid_.download_table_view->item(i, 0)->text();
		QString nodeIDStr;

		if (!getStringBySeparator(nodeIDStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
		{
			return;
		}

		bool ok = false;
		uint8_t node_id = nodeIDStr.toUInt(&ok);

		if (!ok)
		{
			return;
		}

		update_vcu_bin_task(i, ipNodeIDStr.toStdString(), node_id);
	}
}

void framwork_wid::btn_slot_clicked(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles){
	if (topLeft.column() != 6 && roles.size() == 0){
		return;
	}
	std::string local_ip = NET_ADDRESS;
	std::string ip_str;// = main_wid_.download_table_view->item(row, 0)->text.toLocal8Bit();/*= view_model_->item(topLeft.row(), 0)->text().toLocal8Bit()*/
	int port = main_wid_.line_port->text().toInt();
	uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
	uint8_t node_id = main_wid_.edit_node_id->text().toUShort();

	//根据按钮属性，分别进行不同的操作
	switch (roles[0])
	{
	case Btn_ReGet:
		get_vcu_cpu_task(local_ip, ip_str, port, serial_id, node_id);
		break;
	case Btn_Retry:
		//update_vcu_bin_task(ip_str);
		break;
	case Btn_Degrade:
		//update_vcu_bin_task(ip_str);
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
		task->regiset_callback(std::bind(&framwork_wid::update_net_recv_data, this, std::placeholders::_1, std::placeholders::_2,
			std::placeholders::_3, std::placeholders::_4));
	}
	if (!net_task_spool_){
		return;
	}
	net_task_spool_->post(task);
}

void framwork_wid::get_vcu_info_task(const std::string& lcoal_ip, const std::string& ip, uint8_t node_id, uint8_t serial_id, const int port)
{
	get_vcu_cpu_task(lcoal_ip, ip, port, serial_id, node_id);
	get_vcu_version_task(lcoal_ip, ip, port, serial_id,node_id);
}

void framwork_wid::get_vcu_version_task(const std::string& lcoal_ip, const std::string& ip, const int port, uint8_t serial_id, uint8_t node_id)
{
	std::shared_ptr<net_task> tasktype = nullptr;

	try{
		tasktype = std::make_shared<net_task>(lcoal_ip, ip, port, kmessage_cmd_data_forward_get_software_version, serial_id, node_id);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}

	if (tasktype)
	{
		tasktype->set_firmware_info(firm_info_);
		tasktype->regiset_callback([&](const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status){
			QMetaObject::invokeMethod(this, "get_vcu_version_callback", Qt::BlockingQueuedConnection,
				Q_ARG(const std::string&, ipNodeIDStr),
				Q_ARG(const int, operate_type),
				Q_ARG(const std::string&, data),
				Q_ARG(const nsp::proto::errorno_t, status));
		});
	}

	int(net_task::*func)() = &net_task::get_vcu_version;
	gdp::core::GdpSingleton<gdp::gui::WatcherDialog>::instance()->run(tasktype.get(), func, this);
}

void framwork_wid::get_vcu_cpu_task(const std::string& lcoal_ip, const std::string& ip, const int port, uint8_t serial_id,uint8_t node_id )
{
	std::shared_ptr<net_task> taskVersion = nullptr;

	try{
		taskVersion = std::make_shared<net_task>(lcoal_ip, ip, port, kmessage_cmd_data_forward_get_hardware_type, serial_id, node_id);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}

	if (taskVersion)
	{
		taskVersion->set_firmware_info(firm_info_);
		taskVersion->regiset_callback([&](const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status){
			QMetaObject::invokeMethod(this, "get_vcu_type_callback", Qt::BlockingQueuedConnection,
				Q_ARG(const std::string&, ipNodeIDStr),
				Q_ARG(const int, operate_type),
				Q_ARG(const std::string&, data),
				Q_ARG(const nsp::proto::errorno_t, status));
		});
	}

	int(net_task::*func)() = &net_task::get_vcu_type;
	gdp::core::GdpSingleton<gdp::gui::WatcherDialog>::instance()->run(taskVersion.get(), func, this);
}

//创建任务，放进线程池中
void framwork_wid::update_vcu_bin_task(int row, const std::string& ip_node_id_str, uint8_t node_id){
	const QString &ipNodeIDStr = QString::fromStdString(ip_node_id_str);
	QString ipStr;

	if (!getStringBySeparator(ipStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Ip), LINE_SEPARATOR))
	{
		return;
	}

	const std::string &ip = ipStr.toStdString();
	std::shared_ptr<net_task_download> task = nullptr;

	try{
		uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
		task = std::make_shared<net_task_download>(ip, firm_info_.reset_wait_time_, firm_info_.block_write_delay_, serial_id,node_id);
	}
	catch (...){
		nsperror << "new net_task_download make shared failed!";
		return;
	}

	if (task)
	{
		task->regiset_get_vcu_callback([&](const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status){
			QMetaObject::invokeMethod(this, "update_vcu_bin_task_callback", Qt::BlockingQueuedConnection, 
				Q_ARG(const std::string&, ipNodeIDStr),
				Q_ARG(const int, operate_type),
				Q_ARG(const std::string&, data),
				Q_ARG(const nsp::proto::errorno_t, status));
		});
	}

	void(net_task_download::*func)() = &net_task_download::on_task;
	gdp::core::GdpSingleton<gdp::gui::WatcherDialog>::instance()->run(task.get(), func, this);
	main_wid_.download_table_view->item(row, 3)->setText(QString::fromLocal8Bit("正在获取信息..."));
	main_wid_.download_table_view->item(row, 1)->setText("");
	main_wid_.download_table_view->item(row, 2)->setText("");
	nsp::toolkit::singleton<network_client_manager>::instance()->clean_session();
	const std::string &local_ip = NET_ADDRESS;
	int port = main_wid_.line_port->text().toInt();
	start_data_forward(local_ip, ip_node_id_str, port);
	btn_restart_slot(row);
	main_wid_.download_table_view->item(row, 3)->setText(QString::fromLocal8Bit("已完成"));
}

void framwork_wid::upload_file_map(QString index){
	std::string local_ip = NET_ADDRESS;
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

void framwork_wid::update_reset_final_lstatus()
{
	QTableWidgetItem* item = main_wid_.download_table_view->item(index_value_, 5);
	item->setData(Qt::DisplayRole, reset_status_);
}

//void framwork_wid::update_status(QString qstr_staus)
//{
//
//}

void framwork_wid::start_data_forward(const std::string& lcoal_ip, const std::string& ipNodeIDStr, const int port)
{
	QString ipStr;

	if (!getStringBySeparator(ipStr, QString::fromStdString(ipNodeIDStr), static_cast<int>(IpNodeIDField::INIF_Ip), LINE_SEPARATOR))
	{
		return;
	}

	QString nodeIDStr;

	if (!getStringBySeparator(nodeIDStr, QString::fromStdString(ipNodeIDStr), static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
	{
		return;
	}

	uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
	uint8_t node_id = nodeIDStr.toUShort();
	std::shared_ptr < net_task> task = nullptr;

	try{
		task = std::make_shared<net_task>(lcoal_ip, ipStr.toStdString(), port, kmessage_cmd_start_data_forward, serial_id, node_id);
	}
	catch (...){
		nsperror << "new net_task make shared failed!";
		return;
	}

	if (task)
	{
		task->regiset_callback([&](const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status){
			QMetaObject::invokeMethod(this, "start_data_foward_callback", Qt::BlockingQueuedConnection,
				Q_ARG(const std::string&, ipNodeIDStr),
				Q_ARG(const int, operate_type),
				Q_ARG(const std::string&, data),
				Q_ARG(const nsp::proto::errorno_t, status));
		});
	}
	
	int(net_task::*func)() = &net_task::start_data_foward;
	gdp::core::GdpSingleton<gdp::gui::WatcherDialog>::instance()->run(task.get(), func, this);
	get_vcu_info_task(lcoal_ip, ipStr.toStdString(), node_id, serial_id, port);
}

void framwork_wid::on_combox_style_changed(const QString & qstr_style)
{
	if (qstr_style.toLower().compare("vcu") == 0)
	{
		main_wid_.combo_serial_id->setEnabled(true);
	}
	else
	{
		main_wid_.combo_serial_id->setEnabled(false);
	}

	if (main_wid_.edit_node_id->text().isEmpty() || main_wid_.combo_serial_id->currentText().isEmpty())
	{
		main_wid_.connect->setEnabled(false);
	}
	else
	{
		main_wid_.connect->setEnabled(true);
	}

	return;
}

void framwork_wid::on_combox_serial_id_changed(const QString & qstr_serial_id)
{
	return;
}

void framwork_wid::update_net_recv_data(const std::string& ip, const int operate_type, const std::string& data, const nsp::proto::errorno_t status){
	auto iter = map_ip_index_.find(ip);
	if (iter == map_ip_index_.end()){
		return;
	}

	if (operate_type == kmessage_cmd_data_forward_get_hardware_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_type, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (operate_type == kmessage_cmd_data_forward_get_hardware_type_upload)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_type_upload, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (operate_type == kmessage_cmd_data_forward_get_software_version)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_version, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (operate_type == kmessage_cmd_data_forward_get_software_version_upload)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_version_upload, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (kmessage_cmd_data_forward_initial_rom_update == operate_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_status, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (kmessage_cmd_data_forward_download_packet == operate_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_progress_download, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (kmessage_cmd_data_forward_download_packet_ex == operate_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_status, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (kmessage_cmd_data_forward_upload_packet == operate_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_progress_upload, QString::fromLocal8Bit(data.c_str()), status);
	}
	else if (kmessage_cmd_data_forward_reset == operate_type)
	{
		emit updatestatussignal(ip.c_str(), kupdatearea_status, QString::fromLocal8Bit(data.c_str()), status);

		if (status == nsp::proto::kSuccessful)
		{
			emit updatestatussignal(ip.c_str(), kupdatearea_reset, QString::fromLocal8Bit(data.c_str()), status);
		}
	}
	else if (operate_type == kmessage_cmd_start_data_forward)
	{
		if (status == nsp::proto::kSuccessful)
		{
			emit updatestatussignal(ip.c_str(), kupdatearea_data_forward, QString::fromLocal8Bit(data.c_str()), status);
		}
		else
		{
			emit updatestatussignal(ip.c_str(), kupdatearea_status, QString::fromLocal8Bit(data.c_str()), status);
		}
	}
}

void framwork_wid::update_status(QString ipNodeIDStr, int uidate_area, QString result, int status)
{
	auto iter = map_ip_index_.find(ipNodeIDStr.toStdString());

	if (map_ip_index_.end() == iter)
	{
		return;
	}

	QString ip;

	if (!getStringBySeparator(ip, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Ip), LINE_SEPARATOR))
	{
		return;
	}

	QString nodeIDStr;

	if (!getStringBySeparator(nodeIDStr, ipNodeIDStr, static_cast<int>(IpNodeIDField::INIF_Node), LINE_SEPARATOR))
	{
		return;
	}

	bool ok = false;
	uint8_t node_id = nodeIDStr.toUInt(&ok);

	if (!ok)
	{
		return;
	}

	QTableWidgetItem * item = nullptr;
	std::string strtext = result.toStdString().c_str();

	switch ((enum ui_update_area)uidate_area)
	{
	case kupdatearea_type:
		if (status == nsp::proto::errorno_t::kSuccessful)
		{
			item = main_wid_.download_table_view->item(iter->second, 4);

			if (item != nullptr)
			{
				if (firm_info_.modules_type_.compare(result.toStdString().c_str()) != 0)
				{
					item->setData(Qt::DisplayRole, QStringLiteral("型号不一致"));
				}
				else
				{
					item->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				}
			}

			int port = main_wid_.line_port->text().toInt();
			std::string local_ip = NET_ADDRESS;
			uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();

			get_vcu_version_task(local_ip, ip.toStdString(), port, serial_id, node_id);
		}

		item = main_wid_.download_table_view->item(iter->second, 1);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_type_upload:
		if (status == nsp::proto::errorno_t::kSuccessful)
		{
			item = main_wid_.upload_table_widget->item(iter->second, 4);

			if (item != nullptr)
			{
				if (firm_info_.modules_type_.compare(result.toStdString().c_str()) != 0)
				{
					item->setData(Qt::DisplayRole, QStringLiteral("型号不一致"));
				}
				else
				{
					item->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				}
			}
		}

		item = main_wid_.upload_table_widget->item(iter->second, 2);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_version_upload:
		if (status == nsp::proto::errorno_t::kSuccessful)
		{
			item = main_wid_.upload_table_widget->item(iter->second, 4);
			if (item != nullptr)
			{
				if (result.toStdString() > firm_info_.firmware_version_)
				{
					item->setData(Qt::DisplayRole, QStringLiteral("版本更高"));
				}
				else
				{
					item->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				}
			}
		}

		item = main_wid_.upload_table_widget->item(iter->second, 3);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_version:
		if ( status == nsp::proto::errorno_t::kSuccessful )
		{
			item = main_wid_.download_table_view->item(iter->second, 4);
			if (item != nullptr)
			{
				if (result.toStdString() > firm_info_.firmware_version_)
				{
					item->setData(Qt::DisplayRole, QStringLiteral("版本更高"));
				}
				else
				{
					item->setData(Qt::DisplayRole, QStringLiteral("读取完成"));
				}
			}
		}

		item = main_wid_.download_table_view->item(iter->second, 2);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_progress_download:
		if ( status == nsp::proto::errorno_t::kSuccessful)
		{
			item = main_wid_.download_table_view->item(iter->second, 3);
			if (item != nullptr)
			{
				QString show_info(QStringLiteral("进度："));
				show_info += result;
				
				item->setData(Qt::DisplayRole, show_info);

				if (result.compare("100%") == 0)
				{
					//Upload_btns *upload = (Upload_btns*)main_wid_.download_table_view->cellWidget(iter->second, 5);
					//upload->ui.delete_->setEnabled(true);
					strtext = "下载完成,正在重启...";
				}
				else
				{
					item = main_wid_.download_table_view->item(iter->second, 5);
					strtext = "正在升级";
				}
			}
		}
		else
		{
			main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("重试"));
		}
		item = main_wid_.download_table_view->item(iter->second, 4);

		if (item != nullptr)
		{
			
			item->setData(Qt::DisplayRole, QString ::fromLocal8Bit(strtext.c_str()));
		}
		break;
	case kupdatearea_progress_upload:
		if (status == nsp::proto::errorno_t::kSuccessful)
		{
			item = main_wid_.upload_table_widget->item(iter->second, 4);
			if (item != nullptr)
			{
				QString show_info(QStringLiteral("进度："));
				show_info += result;

				item->setData(Qt::DisplayRole, show_info);

				if (result.compare("100%") == 0)
				{
					Upload_btns *upload = (Upload_btns*)main_wid_.upload_table_widget->cellWidget(iter->second, 5);
					upload->ui.delete_->setEnabled(true);
					strtext = "上传完成";
				}
				else
				{
					item = main_wid_.download_table_view->item(iter->second, 5);
					strtext = "正在升级";
				}

				if (item != nullptr)
				{
					item->setData(Qt::DisplayRole, strtext.c_str());
				}
			}

		}
		else
		{
			main_wid_.download_table_view->item(iter->second, 5)->setData(Qt::DisplayRole, QStringLiteral("重试"));
		}
		item = main_wid_.download_table_view->item(iter->second, 4);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_status:
		item = main_wid_.download_table_view->item(iter->second, 4);

		if (item != nullptr)
		{
			item->setData(Qt::DisplayRole, strtext.c_str());
		}
		break;
	case kupdatearea_reset:
	{
		reset_wait_.wait(firm_info_.reset_wait_time_);
		int port = main_wid_.line_port->text().toInt();
		std::string local_ip = NET_ADDRESS;
		uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();
		get_vcu_version_task(local_ip, ip.toStdString(), port, serial_id,node_id);
		break;
	}
	case kupdatearea_data_forward:
	{
		int port = main_wid_.line_port->text().toInt();
		std::string local_ip = NET_ADDRESS;
		uint8_t serial_id = main_wid_.combo_serial_id->currentText().toUShort();

		get_vcu_cpu_task(local_ip, ip.toStdString(), port, serial_id, node_id);
		//get_vcu_info_task(local_ip, ip.toStdString(), port);
		break;
	}
	default:
		break;
	}
}

Q_INVOKABLE void framwork_wid::start_data_foward_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status)
{
	auto iter = map_ip_index_.find(ipNodeIDStr);

	if (map_ip_index_.end() == iter)
	{
		return;
	}

	int row = iter->second;
	QTableWidgetItem *pItem = main_wid_.download_table_view->item(row, 4);

	if (nullptr == pItem)
	{
		return;
	}

	pItem->setText(QString::fromLocal8Bit(data.data()));
}

Q_INVOKABLE void framwork_wid::get_vcu_type_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status)
{
	auto iter = map_ip_index_.find(ipNodeIDStr);

	if (map_ip_index_.end() == iter)
	{
		return;
	}

	int row = iter->second;
	QTableWidgetItem *pItem = main_wid_.download_table_view->item(row, 1);

	if (nullptr == pItem)
	{
		return;
	}

	pItem->setText(QString::fromLocal8Bit(data.data()));
}

Q_INVOKABLE void framwork_wid::get_vcu_version_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status)
{
	auto iter = map_ip_index_.find(ipNodeIDStr);

	if (map_ip_index_.end() == iter)
	{
		return;
	}

	int row = iter->second;
	QTableWidgetItem *pItem = main_wid_.download_table_view->item(row, 2);

	if (nullptr == pItem)
	{
		return;
	}

	pItem->setText(QString::fromLocal8Bit(data.data()));
}

Q_INVOKABLE void framwork_wid::update_vcu_bin_task_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t /*status*/)
{
	auto iter = map_ip_index_.find(ipNodeIDStr);

	if (map_ip_index_.end() == iter)
	{
		return;
	}

	int row = iter->second;
	QTableWidgetItem *pItem = main_wid_.download_table_view->item(row, 3);

	if (nullptr == pItem)
	{
		return;
	}

	pItem->setText(QString::fromLocal8Bit(data.data()));
}
