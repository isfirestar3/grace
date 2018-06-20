#ifndef NETWORKSETTINGWIDGET_H
#define NETWORKSETTINGWIDGET_H

#include "ui_networksettingwidget.h"
#include "mntypes.h"
#include <QtWidgets/QWidget>
#include <functional>
#include <QtCore\QTimer>
#include "view_net.h"

//class wid_network_setting;

class NetworkSettingWidget : public QWidget
{
	Q_OBJECT

	struct UserData : QObjectUserData {
		UserData(int row){ nRow_ = row; };
		int nRow_;
	};
public:
	NetworkSettingWidget(QWidget *parent = nullptr);
	~NetworkSettingWidget();

	static void notify();

	void InitUi();
	void set_setting_callbck(const std::function<void(int, int)>& func);//回调函数，回调当前网络状态给上层类，此处回调给net_setting类；//参数一：网络状态，参数二：机器人ID
	//void register_rece_callback(uint32_t id, void *data, int type);
	void UpdateNetworkStatus(mn::net_status_t net_status,bool bDhcp = false);
	void SetApplicationNotify();
	void OnConnectToHost(const QString &ip, int port, bool bDhcp = false);
	void ArgsOnConnectToHost(const QString &ip, int port);
	void OnDisconnectToHost(bool bDhcp = false);
	Q_INVOKABLE void UpdateConnectState(bool connected);
	Q_INVOKABLE void UpdateConnectChangedState(bool connected);

	Q_INVOKABLE void UpdateConnectBtnStatus(bool connected);
signals:
	void NotifyToUpdateConnectState(bool, const QString &, int);
public slots:
	void on_connectBtn_clicked();
	void update_network_status();
	void on_radio_manual_clicked();
	void on_radio_dhcp_clicked();
	void on_connect_dhcp_btn_clicked();
	void stateChanged_checkbox(int ncheck);

	void on_btn_connect_clicked();

	void update_table_view();
private:
	Ui::NetworkSettingWidget ui;
	uint32_t m_robotId{ -1 };
	mn::net_status_t m_net_status{mn::kNetworkStatus_Closed};
	//wid_network_setting *m_wid_network_setting;
	std::function<void(int, int)> m_net_status_callback;//参数一：网络状态，参数二：机器人ID
	QString m_prevIpAddr;
	int m_prevPort{-1};
	QTimer* query_task_;
private:
	void query_network_status();

	void RefreshWindow();

	void clear_agvinfo();

	static NetworkSettingWidget * pSettingWidget_ ;
};
#endif // NETWORKSETTINGWIDGET_H
