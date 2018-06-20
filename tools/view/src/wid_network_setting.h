#pragma once

#include <QtWidgets\qlabel.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qgridlayout.h>
#include <QtCore\QTimer>
#include <QtCore\QSharedPointer>

#include "networksettingwidget.h"
#include "push_button.h"
#include "os_util.hpp"
#include "data_type.h"
#include "endpoint.h"
#include <atomic>

#include "mntypes.h"

struct __ipport{
	std::string ip_;
	int port_;
	//connect_type_t type_;
};

class wid_network_setting : public QWidget{
	Q_OBJECT
	friend NetworkSettingWidget;
public:
	wid_network_setting(QWidget* parent=0);
	~wid_network_setting();

	void set_setting_callbck(const std::function<void(int,int)>& func);//回调函数，回调当前网络状态给上层类，此处回调给net_setting类；//参数一：网络状态，参数二：机器人ID
	
	NetworkSettingWidget *GetNetworkSettingWidget() const
	{
		return m_netSetWidget.data();
	}
private slots :
	void combobox_index_change(int index);
	void connect_net();
	void slot_update_status();

private:
	void init_form();
	void init_slot();

	void init_net_commition();
	void find_ip_port(int ip, int mode, __ipport&tmp);
	void register_rece_callback(uint32_t id, void *data, int type);

private:
	QComboBox* mode_combox_;
	QComboBox* ip_combox_;
	QPushButton* connect_btn_;
	QTimer* __timer_ip;
	QSharedPointer<NetworkSettingWidget> m_netSetWidget;

	std::vector<__ipport> vct_ipport_;
	std::vector<__ipport> vct_bc_ipport_;
	nsp::tcpip::endpoint __discover_ep;

	std::atomic<int> __net_status = mn::kNetworkStatus_Closed; //记录网络状态
	int32_t robot_id_;//机器人编号

	std::function<void(int,int)> net_status_callback_;//参数一：网络状态，参数二：机器人ID
};