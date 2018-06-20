#include "networksettingwidget.h"
//#include "wid_network_setting.h"
#include "common_impls.hpp"
#include "common/watcherwaiter.hpp"
#include "dbg_vars_impls.h"
#include "net_impls.h"
#include "net_task.h"
#include "application.h"
#include "common/MessageBox.h"
#include "libagvinfo.h"
#include <QtWidgets/QCheckBox>

NetworkSettingWidget* NetworkSettingWidget::pSettingWidget_ = nullptr;

extern agv_api agv_interface_single_;

NetworkSettingWidget::NetworkSettingWidget(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	InitUi();
	pSettingWidget_ = this;
	/*m_wid_network_setting = qobject_cast<wid_network_setting *>(parent);
	Q_ASSERT(nullptr != m_wid_network_setting);*/
}

NetworkSettingWidget::~NetworkSettingWidget()
{

}

void NetworkSettingWidget::notify()
{
	QTimer::singleShot(0, pSettingWidget_, SLOT(update_table_view()));
}

void NetworkSettingWidget::InitUi()
{
	ui.radio_manual->setText(IS_EN ? "Manually enter the IP address" : "手动输入IP地址");
	ui.ipLabel->setText(IS_EN ? "Ip:" : "IP地址：");
	ui.portLabel->setText(IS_EN ? "Port:" : "端口：");
	ui.statusLabel->setText(IS_EN ? "Status:" : "状态：");
	ui.connectBtn->setText(IS_EN ? "Connect" : "连接");
	ui.radio_dhcp->setText(IS_EN ? "Get the IP on the DHCP server":"从DHCP服务器中选择IP地址");
	ui.ipLabel_dhcp->setText(IS_EN ? "Ip:" : "IP地址：");
	ui.portLabel_dhcp->setText(IS_EN ? "Port:" : "端口：");
	ui.connect_dhcp_btn->setText(IS_EN ? "Obtain List" : "获取");
	ui.btn_connect->setText(IS_EN ? "Connect" : "连接");

	QPalette palette;
	palette.setColor(QPalette::Background, QColor(221, 225, 230));
	setAutoFillBackground(true);
	setPalette(palette);

	UpdateNetworkStatus(m_net_status);

	//添加定时器，定时刷新网络链接状态
	query_task_ = new QTimer(this);
	connect(query_task_, SIGNAL(timeout()), this, SLOT(update_network_status()));
	query_task_->start(1000);
	RefreshWindow();

	about_something_changed(notify);

	ui.tableWidget_agvinfo->setEditTriggers(QAbstractItemView::NoEditTriggers);//表格不可编辑
	ui.tableWidget_agvinfo->setSelectionBehavior(QAbstractItemView::SelectRows);//表格选中一行
	ui.tableWidget_agvinfo->verticalHeader()->setHidden(true);
	//ui.tableWidget_agvinfo->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);    //x先自适应宽度
	ui.tableWidget_agvinfo->setSelectionMode(QAbstractItemView::SingleSelection);
	//ui.tableWidget_agvinfo->horizontalHeader()->setSectionResizeMode(2, QHeaderView::ResizeToContents);
	//ui.tableWidget_agvinfo->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);
}

void NetworkSettingWidget::set_setting_callbck(const std::function<void(int, int)>& func)
{
	m_net_status_callback = func;
}

void NetworkSettingWidget::RefreshWindow()
{
	bool checked = ui.radio_manual->isChecked();

	if (checked){
		ui.ipLineEdit->setEnabled(true);
		ui.portLineEdit->setEnabled(true);
		ui.connectBtn->setEnabled(true);

		ui.tableWidget_agvinfo->setEnabled(false);
		ui.ipLineEdit_dhcp->setEnabled(false);
		ui.portLineEdit_dhcp->setEnabled(false);
		ui.connect_dhcp_btn->setEnabled(false);
		ui.btn_connect->setEnabled(false);
	}
	else{
		ui.ipLineEdit->setEnabled(false);
		ui.portLineEdit->setEnabled(false);
		ui.connectBtn->setEnabled(false);

		ui.tableWidget_agvinfo->setEnabled(true);
		ui.ipLineEdit_dhcp->setEnabled(true);
		ui.portLineEdit_dhcp->setEnabled(true);
		ui.connect_dhcp_btn->setEnabled(true);

		for (int index = 0; index < ui.tableWidget_agvinfo->rowCount(); index++){
			QWidget*widget = ui.tableWidget_agvinfo->cellWidget(index, 0);
			if (widget){
				QLayout * layout = widget->layout();
				if (layout){
					QCheckBox * pcheck = (QCheckBox *)layout->itemAt(0)->widget();
					if (pcheck->isChecked()){
						ui.btn_connect->setEnabled(true);
						return;
					}
				}
			}
		}
		ui.btn_connect->setEnabled(false);
	}
}

//void NetworkSettingWidget::register_rece_callback(uint32_t id, void *data, int type)
//{
//	switch (type)
//	{
//	case kNET_STATUS:
//	{
//		m_net_status = *(net_status_t*)data;
//		UpdateNetworkStatus(static_cast<net_status_t>(m_net_status));
//
//		if (m_net_status_callback){
//			m_net_status_callback(m_net_status, m_robotId);
//		}
//	}
//		break;
//	default:
//		break;
//	}
//}

void NetworkSettingWidget::UpdateNetworkStatus(mn::net_status_t net_status, bool bDhcp)
{

	m_net_status = net_status;
	bool connected = false;

	if (mn::kNetworkStatus_Connected <= m_net_status)
	{
		connected = true;
	}

	if (!bDhcp){
		QMetaObject::invokeMethod(this, "UpdateConnectState", Qt::QueuedConnection,
			QGenericReturnArgument(),
			Q_ARG(bool, connected));
	}
	else{
		QMetaObject::invokeMethod(this, "UpdateConnectBtnStatus", Qt::QueuedConnection,
			QGenericReturnArgument(),
			Q_ARG(bool, connected));
	}

}

void NetworkSettingWidget::SetApplicationNotify()
{
	int argc = nsp::toolkit::singleton<Application>::instance()->GetArgc();

	if (1 >= argc)
	{
		return;
	}

	connect(nsp::toolkit::singleton<Application>::instance(), &Application::ConnectToHost, this, &NetworkSettingWidget::ArgsOnConnectToHost);

	if (!nsp::toolkit::singleton<Application>::instance()->Parse())
	{
		nsperror << __FUNCTION__ << "Parse the application args failed!";

		exit(-1);
	}

	nspinfo << __FUNCTION__ << "Parse the application args successed!";
}

void NetworkSettingWidget::OnConnectToHost(const QString &ip, int port, bool bDhcp)
{
	if (mn::kNetworkStatus_Connected == m_net_status){
		if (ip == m_prevIpAddr && port == port){
			return;
		}
		else{
			OnDisconnectToHost(bDhcp);
		}
	}

	bool(net_task::*func)(const QString &, int, uint32_t &, std::function<void(uint32_t, void *, int)>) = &net_task::connect_to_host;
	std::function<void(uint32_t, void *, int)> dataCb;// = std::bind(&wid_network_setting::register_rece_callback, m_wid_network_setting, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3);
	net_task nt;
	std::function<void(bool)> retCb = [&](bool successed)->void{
		if (successed)
		{
			m_net_status = mn::kNetworkStatus_Connected;
			m_prevIpAddr = ip;
			m_prevPort = port;

			if (nullptr != m_net_status_callback){
				m_net_status_callback(m_net_status, m_robotId);
			}
		}
		else
		{
			CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Connect failed!" : "连接失败！", CMessageBox::Ok);
		}

		UpdateNetworkStatus(m_net_status, bDhcp);
	};

	nsp::toolkit::singleton<WatcherDialog>::instance()->run(&nt, func, ip, port, std::ref(m_robotId), dataCb, retCb, getTopWidget());
}

void NetworkSettingWidget::ArgsOnConnectToHost(const QString &ip, int port)
{
	ui.ipLineEdit->setText(ip);
	ui.portLineEdit->setText(QString::number(port));
	OnConnectToHost(ip, port);
}

void NetworkSettingWidget::OnDisconnectToHost(bool bDhcp)
{
	bool(net_task::*func)(uint32_t) = &net_task::disconnect_to_host;
	net_task nt;
	std::function<void(bool)> retCb = [&](bool successed)->void{
		if (successed)
		{
			m_net_status = mn::kNetworkStatus_Closed;
			m_prevIpAddr = "";
			m_prevPort = -1;

			if (nullptr != m_net_status_callback){
				m_net_status_callback(m_net_status, m_robotId);
			}
		}
		else
		{
			CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Disconnect failed!" : "断开失败！", CMessageBox::Ok);
		}

		UpdateNetworkStatus(m_net_status, bDhcp);
	};

	nsp::toolkit::singleton<WatcherDialog>::instance()->run(&nt, func, m_robotId, retCb, getTopWidget());
}

Q_INVOKABLE void NetworkSettingWidget::UpdateConnectState(bool connected)
{
	if (!connected)
	{
		ui.statusLabel_->setText(IS_EN ? "Disconnected" : "未连接");
		ui.connectBtn->setText(IS_EN ? "Connect" : "连接");
		ui.ipLineEdit->setEnabled(true);
		ui.portLineEdit->setEnabled(true);
		emit NotifyToUpdateConnectState(false, QString(), -1);
	}
	else
	{
		ui.statusLabel_->setText(IS_EN ? "Connected" : "已连接");
		ui.connectBtn->setText(IS_EN ? "Disconnect" : "断开");
		ui.ipLineEdit->setEnabled(false);
		ui.portLineEdit->setEnabled(false);
		emit NotifyToUpdateConnectState(true, m_prevIpAddr, m_prevPort);
	}
}

Q_INVOKABLE void NetworkSettingWidget::UpdateConnectBtnStatus(bool connected)
{
	if (!connected){
		ui.btn_connect->setText(IS_EN ? "Connect" : "连接");
		emit NotifyToUpdateConnectState(false, QString(), -1);
	}
	else{
		ui.btn_connect->setText(IS_EN ? "Disconnect" : "断开");
		emit NotifyToUpdateConnectState(true, m_prevIpAddr, m_prevPort);
	}
}

void NetworkSettingWidget::update_network_status()
{
	int status = -1;
	agv_interface_single_.get_net_status(status);
	if (m_net_status_callback)
	{
		m_net_status_callback(status,0);
	}
}

void NetworkSettingWidget::on_connectBtn_clicked()
{
	QString ip = ui.ipLineEdit->text();

	if (ip.isEmpty())
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Please input the IP addr!" : "请输入IP地址！", CMessageBox::Ok);
		ui.ipLineEdit->setFocus();

		return;
	}

	QString portStr = ui.portLineEdit->text();

	if (portStr.isEmpty())
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Please input the port!" : "请输入端口！", CMessageBox::Ok);
		ui.portLineEdit->setFocus();

		return;
	}

	bool ok = false;
	int port = portStr.toInt(&ok);

	if (!ok)
	{
		return;
	}

	if (mn::kNetworkStatus_Connected > m_net_status)
	{
		OnConnectToHost(ip, port);
	}
	else
	{
		OnDisconnectToHost();
	}
}

void NetworkSettingWidget::stateChanged_checkbox(int ncheck){
	QCheckBox *checkbox = (QCheckBox*)sender();
	if (Qt::Checked == ncheck){
		UserData * data = (UserData*)(checkbox->userData(Qt::UserRole));
		if (data){
			int row = data->nRow_;
			for (int index = 0; index < ui.tableWidget_agvinfo->rowCount(); index++){
				if (index == row){
					continue;
				}
				QWidget*widget = ui.tableWidget_agvinfo->cellWidget(index, 0);
				if (widget){
					QLayout * layout = widget->layout();
					if (layout){
						QCheckBox * pcheck = (QCheckBox *)layout->itemAt(0)->widget();
						if (pcheck->isChecked()){
							pcheck->setChecked(false);
						}
					}
				}
			}
		}
		ui.btn_connect->setEnabled(true);
	}
	else{
		ui.btn_connect->setEnabled(false);
	}
}

Q_INVOKABLE void NetworkSettingWidget::UpdateConnectChangedState(bool connected)
{
	if (connected)
	{
		OnDisconnectToHost();
		OnConnectToHost(m_prevIpAddr, m_prevPort);
	}
	else
	{
		UpdateConnectState(false);
	}
}

void NetworkSettingWidget::on_radio_manual_clicked()
{
	RefreshWindow();
	disconnect_from_agvinfo();
	clear_agvinfo();
	if (!( mn::kNetworkStatus_Connected > m_net_status))
	{
		OnDisconnectToHost(true);
	}
}

void NetworkSettingWidget::on_radio_dhcp_clicked()
{
	RefreshWindow();
	if (!(mn::kNetworkStatus_Connected > m_net_status))
	{
		OnDisconnectToHost();
	}
}

void NetworkSettingWidget::on_connect_dhcp_btn_clicked()
{
	QString ip = ui.ipLineEdit_dhcp->text();
	if (ip.isEmpty())
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Please input the IP addr!" : "请输入IP地址！", CMessageBox::Ok);
		ui.ipLineEdit_dhcp->setFocus();

		return;
	}

	QString portStr = ui.portLineEdit_dhcp->text();

	if (portStr.isEmpty())
	{
		CMessageBox::Critical(this, IS_EN ? "Critical" : "错误", IS_EN ? "Please input the port!" : "请输入端口！", CMessageBox::Ok);
		ui.portLineEdit_dhcp->setFocus();

		return;
	}

	if (connect_agvinfo_server(ip.toStdString().c_str(), portStr.toInt())){
		CMessageBox::Critical(this, IS_EN ? "Warning" : "警告", IS_EN ? "Connect failed!" : "连接失败！", CMessageBox::Ok);
		return;
	}

	update_table_view();
}

void NetworkSettingWidget::on_btn_connect_clicked()
{
	QString ipstr;
	QString portstr;
	for (int index = 0; index < ui.tableWidget_agvinfo->rowCount(); index++){
		QWidget*widget = ui.tableWidget_agvinfo->cellWidget(index, 0);
		if (widget){
			QLayout * layout = widget->layout();
			if (layout){
				QCheckBox * pcheck = (QCheckBox *)layout->itemAt(0)->widget();
				if (pcheck->isChecked()){
					QTableWidgetItem* veip = ui.tableWidget_agvinfo->item(index, 2);
					QString strip = veip->text();
					ipstr = strip.section(':', 0,0);
					portstr = strip.section(':', -1);
					break;
				}
			}
		}
	}

	bool ok = false;
	int port = portstr.toInt(&ok);

	if (!ok){
		return;
	}

	if (mn::kNetworkStatus_Connected > m_net_status){
		OnConnectToHost(ipstr, port,true);
	}
	else{
		OnDisconnectToHost(true);
	}
}


void NetworkSettingWidget::update_table_view()
{
	agv_info * p_agvinfo = nullptr;
	if (load_agvinfo(&p_agvinfo, LAM_Real)){
		return;
	}

	clear_agvinfo();
	int row = 0;
	for (auto iter = p_agvinfo; iter != nullptr; iter = iter->next){
		if (!nsp::tcpip::endpoint::is_effective_ipv4(iter->inet) || iter->status != AS_ONLINE){
			continue;
		}

		ui.tableWidget_agvinfo->insertRow(row);
		QWidget *widget = new QWidget(ui.tableWidget_agvinfo);
		QCheckBox * pCheckBox = new QCheckBox();
		connect(pCheckBox, SIGNAL(stateChanged(int)), SLOT(stateChanged_checkbox(int)));
		if (!m_prevIpAddr.compare(iter->inet) && iter->mtport == m_prevPort){
			pCheckBox->setCheckState(Qt::Checked);
		}
		else{
			pCheckBox->setCheckState(Qt::Unchecked);
		}
		
		UserData * data = new UserData(row);
		pCheckBox->setUserData(Qt::UserRole, data);
		QHBoxLayout *hLayout= new QHBoxLayout();
		hLayout->addWidget(pCheckBox);
		hLayout->setMargin(0);                          // 必须添加, 否则CheckBox不能正常显示
		hLayout->setAlignment(pCheckBox, Qt::AlignCenter);
		widget->setLayout(hLayout);
		ui.tableWidget_agvinfo->setCellWidget(row, 0, widget);

		QTableWidgetItem* veid = new QTableWidgetItem;
		char ve_id[10];
		veid->setText(QString::fromLocal8Bit(itoa(iter->vhid, ve_id, 10)));
		ui.tableWidget_agvinfo->setItem(row, 1, veid);
		ui.tableWidget_agvinfo->item(row, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);

		QTableWidgetItem* veip = new QTableWidgetItem;
		QString strip(QString::fromLocal8Bit(iter->inet));
		strip += ":" + QString::number(iter->mtport, 10);
		veip->setText(strip);
		ui.tableWidget_agvinfo->setItem(row, 2, veip);
		ui.tableWidget_agvinfo->item(row, 2)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//居中显示

		QTableWidgetItem* vemac = new QTableWidgetItem;
		vemac->setText(QString::fromLocal8Bit(iter->hwaddr));
		ui.tableWidget_agvinfo->setItem(row, 3, vemac);
		ui.tableWidget_agvinfo->item(row, 3)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);//居中显示
		row++;
	};
	release_agvinfo(p_agvinfo);
}

void NetworkSettingWidget::clear_agvinfo(){
	for (int index = 0; index < ui.tableWidget_agvinfo->rowCount(); index++){
		QWidget*widget = ui.tableWidget_agvinfo->cellWidget(index, 0);
		if (widget){
			QLayout * layout = widget->layout();
			if (layout){
				for (int num = 0; num < layout->count(); num++){
					QLayoutItem *it = layout->itemAt(num);
					QWidget *widget = it->widget();
					if (widget){
						//QObjectUserData * data = widget->userData(Qt::UserRole);
						//if (data){
						//	delete data;
						//}
						delete widget;
					}
				}
				delete layout;
			}
			delete widget;
		}

		QTableWidgetItem *vhid = ui.tableWidget_agvinfo->item(index,1);
		if (vhid){
			delete vhid;
		}
		QTableWidgetItem* veip = ui.tableWidget_agvinfo->item(index, 2);
		if (veip){
			delete veip;
		}
		QTableWidgetItem* vemac = ui.tableWidget_agvinfo->item(index, 3);
		if (vemac){
			delete vemac;
		}
	}
	ui.tableWidget_agvinfo->clearContents();
	ui.btn_connect->setEnabled(false);
	ui.tableWidget_agvinfo->setRowCount(0);
}
