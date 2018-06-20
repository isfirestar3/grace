#include "wid_network_setting.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <iostream>
#include "log.h"

#include "view_net.h"

wid_network_setting::wid_network_setting(QWidget* parent)
:m_netSetWidget{QSharedPointer<NetworkSettingWidget>::create(this)}
{
	init_form();
	init_slot();
	init_net_commition();
}

wid_network_setting::~wid_network_setting(){

}

void wid_network_setting::init_form(){
	QHBoxLayout* h_layout = new QHBoxLayout;
	QLabel* label_mode = new QLabel(QStringLiteral("模式:"));
	label_mode->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	mode_combox_ = new QComboBox;
	mode_combox_->insertItem(0, QStringLiteral("请选择模式"));
	mode_combox_->insertItem(1, QStringLiteral("定向连接"));
	mode_combox_->insertItem(2, QStringLiteral("广播连接"));
	QLabel* label_ip = new QLabel(QStringLiteral("IP:"));
	label_ip->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
	ip_combox_ = new QComboBox;
	ip_combox_->setEditable(true);
	connect_btn_ = new QPushButton(QStringLiteral("连接"));
	h_layout->addWidget(label_mode);
	h_layout->addWidget(mode_combox_);
	h_layout->addWidget(label_ip);
	h_layout->addWidget(ip_combox_);
	h_layout->setStretchFactor(label_mode, 1);
	h_layout->setStretchFactor(mode_combox_, 5);
	h_layout->setStretchFactor(label_ip, 1);
	h_layout->setStretchFactor(ip_combox_, 5);

	QWidget* wid_left = new QWidget;
	QHBoxLayout* bttom_layout = new QHBoxLayout;
	bttom_layout->addWidget(wid_left);
	bttom_layout->addWidget(connect_btn_);
	bttom_layout->setStretchFactor(wid_left, 4);
	bttom_layout->setStretchFactor(connect_btn_, 1);
	QWidget* wid_bttom = new QWidget;

	QVBoxLayout*v_layout = new QVBoxLayout;
	//v_layout->addLayout(h_layout);
	//v_layout->addLayout(bttom_layout);
	//v_layout->addWidget(wid_bttom);
	//v_layout->setStretchFactor(h_layout, 1);
	//v_layout->setStretchFactor(bttom_layout, 1);
	//v_layout->setStretchFactor(wid_bttom, 4);
	v_layout->addWidget(m_netSetWidget.data());
	v_layout->setContentsMargins(0, 0, 0, 0);
	this->setLayout(v_layout);

	__timer_ip = new QTimer(this);
}

void wid_network_setting::init_slot(){
	connect(mode_combox_, SIGNAL(currentIndexChanged(int)), this, SLOT(combobox_index_change(int)));
	connect(connect_btn_, SIGNAL(clicked()), this, SLOT(connect_net()));
	connect(__timer_ip, SIGNAL(timeout()), this, SLOT(slot_update_status()));
	__timer_ip->start(500);
}

void wid_network_setting::init_net_commition()
{
	std::string file_path = nsp::os::get_module_directory<char>();
	file_path += "\\etc\\net_setting.xml";

	rapidxml::xml_document<> doc;
	rapidxml::file<> *file = nullptr;
	try{
		file = new rapidxml::file<>(file_path.c_str());
		doc.parse<0>(file->data());
		rapidxml::xml_node<>* root = doc.first_node();
		if (root)
		{
			rapidxml::xml_node<>* anode = root->first_node();
			if (NULL == anode)
			{
				delete file;
				return;
			}
			rapidxml::xml_node<>*bnode = anode->first_node();
			for (; bnode != NULL; bnode = bnode->next_sibling()){
				rapidxml::xml_attribute<char> * attr = bnode->first_attribute();
				if (NULL != attr)
				{
					__ipport tmp;
					tmp.ip_ = attr->value();
					attr = attr->next_attribute();
					if (NULL != attr)
					{
						tmp.port_ = atoi(attr->value());
					}
					tmp.type_ = KConnectType_Manual;
					vct_ipport_.push_back(tmp);
				}
			}
			anode = anode->next_sibling();
			if (NULL != anode){
				bnode = anode->first_node();
				if (NULL != bnode)
				{
					rapidxml::xml_attribute<char> * attr = bnode->first_attribute();
					if (NULL != attr){
						__ipport tmp;
						tmp.ip_ = attr->value();
						attr = attr->next_attribute();
						if (NULL != attr){
							tmp.port_ = atoi(attr->value());
						}
						tmp.type_ = KConnectType_Broadcast;
						vct_ipport_.push_back(tmp);
						__discover_ep.ipv4(tmp.ip_);
						__discover_ep.port(tmp.port_);
					}
				}
			}
		}
		delete file;
	}
	catch (...)
	{
		delete file;
	}
}

void wid_network_setting::combobox_index_change(int index)
{
	ip_combox_->clear();
	if (index <= 0) {
		return;
	}
	if (2 == index){
		int count = 0;
		for (auto iter : vct_ipport_){
			if (KConnectType_Broadcast == iter.type_){
				ip_combox_->insertItem(count, QString::fromLocal8Bit(iter.ip_.c_str()));
			}
		}
	}
	else if (1 == index){
		int count = 0;
		for (auto iter : vct_ipport_){
			if (KConnectType_Manual == iter.type_){
				ip_combox_->insertItem(count, QString::fromLocal8Bit(iter.ip_.c_str()));
				count++;
			}
		}
	}
}

void wid_network_setting::connect_net()
{
	if (__net_status >= mn::kNetworkStatus_Connected) {
		nsp::toolkit::singleton<viewer_net>::instance()->logout();
		//disconnect_host(robot_id_);
		__net_status = mn::kNetworkStatus_Closed;
		connect_btn_->setText(QStringLiteral("连接"));
		return;
	}

	__ipport tmp;
	int ip = ip_combox_->currentIndex();
	int mode = mode_combox_->currentIndex();
	find_ip_port(ip, mode, tmp);

	//nsp::tcpip::endpoint ep(tmp.ip_.c_str(), tmp.port_);

	/*if (init_broadcast() < 0){
		nsperror << "failed to broadcast network.";
		return;
	}*/

	//robot_id_ = init_net();

	if (nsp::toolkit::singleton<viewer_net>::instance()->login(tmp.ip_, tmp.port_) < 0)
	{
		nsperror << "failed to connect to target endpoint:" << tmp.ip_;
		return;
	}

	/*if (login_to_host(robot_id_, ep, kControlorType_RealtimeViewer) < 0){
		return;
	}*/

	//register_callback_to_notify(robot_id_, std::bind(&wid_network_setting::register_rece_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	//循环查询当前链接状态，如果3秒内未发现链接成功，则认为未能连接成功
	for (size_t i = 0; i < 3; i++)
	{
		int status;
		nsp::toolkit::singleton<viewer_net>::instance()->get_net_status(status);
		if (mn::kNetworkStatus_Connected == status)
		{
			connect_btn_->setText(QStringLiteral("断开"));
			__net_status = mn::kNetworkStatus_Connected;
			break;
		}
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	
	/*if (net_status_callback_){
		net_status_callback_(__net_status, robot_id_);
	}*/
	return;
}

void wid_network_setting::find_ip_port(int ip, int mode, __ipport&tmp)
{
	if (0 == mode)return;
	if (1 == mode){
		tmp.ip_ = vct_ipport_[ip].ip_;
		tmp.port_ = vct_ipport_[ip].port_;
		tmp.type_ = vct_ipport_[ip].type_;
	}
	if (2 == mode){
		tmp.ip_ = vct_ipport_[vct_ipport_.size() - 1].ip_;
		tmp.port_ = vct_ipport_[vct_ipport_.size() - 1].port_;
		tmp.type_ = vct_ipport_[vct_ipport_.size() - 1].type_;
	}
}

void wid_network_setting::register_rece_callback(uint32_t id, void *data, int type)
{
	/*switch (type)
	{
	case kNET_STATUS:
	{
		std::cout << "the net is break" << std::endl;
		__net_status = *(net_status_t*)data;

		if (net_status_callback_){
			net_status_callback_(__net_status, robot_id_);
		}

		if (!m_netSetWidget.isNull())
		{
			bool connected = false;

			if (mn::kNetworkStatus_Connected <= __net_status)
			{
				connected = true;
			}

			QMetaObject::invokeMethod(m_netSetWidget.data(), "UpdateConnectChangedState", Qt::QueuedConnection,
				QGenericReturnArgument(), 
				Q_ARG(bool, connected));
		}
	}
	break;
	default:
		break;
	}*/
}

void wid_network_setting::slot_update_status()
{
	switch (__net_status) {
	case mn::kNetworkStatus_Closed:
		connect_btn_->setText(QStringLiteral("连接"));
		break;
	case mn::kNetworkStatus_Connected:
		connect_btn_->setText(QStringLiteral("断开"));
		break;
	default:
		break;
	}
}

void wid_network_setting::set_setting_callbck(const std::function<void(int,int)>& func){
	if (!net_status_callback_){
		net_status_callback_ = func;
	}

	if (m_netSetWidget.isNull())
	{
		return;
	}

	m_netSetWidget->set_setting_callbck(func);
}