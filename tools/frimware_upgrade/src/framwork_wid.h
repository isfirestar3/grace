#pragma once

#include "ui_firmware.h"
#include <QtWidgets\qfiledialog.h>
#include <QtWidgets\QMessageBox.h>
#include <QtGui\QStandardItemModel>
#include <QtCore\QSignalMapper>
#include "config.h"
#include "task_scheduler.hpp"
#include "net_task.h"
#include "net_task_download.h"
#include "net_task_upload.h"
#include "network_server_manager.h"
#include "custom_control_delegate.h"
#include <mutex>

#define NET_WINDOW_SIZE     2

#define NET_ADDRESS	"0.0.0.0"

class QRegExpValidator;

class framwork_wid : public QMainWindow{
	Q_OBJECT
public:
	enum class NodeIDField
	{
		NIF_StartNodeID,
		NIF_EndNodeID, 
		NIF_FieldCount
	};

	enum class IpNodeIDField
	{
		INIF_Ip, 
		INIF_Node,
		INIF_FieldCount
	};
public:
	framwork_wid(QWidget*parent = 0);
	~framwork_wid();

private:
	void init_slot();
	int ip_separate(const std::string&ip_start, const std::string&ip_end, std::vector<std::string>& vct_str);
	int start_task_thead();
	void update_net_recv_data(const std::string& ip, const int operate_type, const std::string& data, const nsp::proto::errorno_t error);
private slots:
	void init_xml_file();
	void action_download();
	void action_upload();
	void exit_application();
	void connect_vcu();
	void update_vcu_firmware();
	void upload_file_map(QString);
	void download_operate_map(QString);
	void add_upload_item();
	void btn_slot_clicked(const QModelIndex &topLeft, const QModelIndex &bottomRight, const QVector<int> &roles);
	void btn_start_slot(int row);
	void btn_restart_slot(int row);
	void btn_reset_slot(int row);
	void btn_start_upload_slot(int row);
	void btn_delete_slot(int row);
	//void update_recv_data();
	void update_reset_final_lstatus();
	void select_all_check();
	void on_combox_style_changed(const QString &qstr_style);
	void on_combox_serial_id_changed(const QString & qstr_serial_id );

	void update_status(QString ip, int uidate_area, QString result, int status);
signals:
	void updatestatussignal(QString ip, int uidate_area, QString result, int status);
private:
	void get_vcu_info_task(const std::string& lcoal_ip, const std::string& ip, uint8_t node_id, uint8_t serial_id, const int port);
	void update_vcu_bin_task(int row, const std::string& ip_node_id_str, uint8_t node_id);
	void get_upload_vcu_info_task(const std::string& lcoal_ip, const std::string& ip, const int port);

	void start_data_forward(const std::string& lcoal_ip, const std::string& ip, const int port);

	void get_vcu_version_task(const std::string& lcoal_ip, const std::string& ip, const int port, uint8_t serial_id, uint8_t node_id);

	void get_vcu_cpu_task(const std::string& lcoal_ip, const std::string& ip, const int port, uint8_t serial_id, uint8_t node_id);

	Q_INVOKABLE void start_data_foward_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status);
	Q_INVOKABLE void get_vcu_type_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status);
	Q_INVOKABLE void get_vcu_version_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status);
	Q_INVOKABLE void update_vcu_bin_task_callback(const std::string& ipNodeIDStr, const int operate_type, const std::string& data, const nsp::proto::errorno_t status);
private:
	Ui::MainWindow main_wid_;
	config config_info_;
	firmware_info firm_info_;
	QSharedPointer<QRegExpValidator> m_nodeIDValidator;
//	QStandardItemModel *view_model_;
	custom_control_delegate* custom_delegate_;
	QSignalMapper* pMapper_;

	//获取状态信息线程池
	std::shared_ptr<nsp::toolkit::task_thread_pool<net_task>> net_task_spool_ = nullptr;
	//下载bin文件线程池
	std::shared_ptr<nsp::toolkit::task_thread_pool<net_task_download>> net_task_download_spool_ = nullptr;
	//上载bin文件线程池
	std::shared_ptr<nsp::toolkit::task_thread_pool<net_task_upload>> net_task_upload_spool_ = nullptr;

	std::recursive_mutex lock_mutex_;//此锁用于锁住不同的线程回调注册的回调函数
	std::map<std::string, int> map_ip_index_;//存放ip地址，用于获取网络数据时进行索引(使用lock_mutex锁保证线程安全性)
	std::map<std::string, std::string> map_ip_path_;
	nsp::os::waitable_handle reset_wait_;
	std::atomic<int> index_value_;
	int radi_;
	std::atomic<QVariant> var_;
	std::atomic<QVariant> reset_status_;
	std::atomic<int> index_delete_;
	enum device_type  type_device_;
	//std::string ip_; 
	//int data_type_;
	//std::string data_;
	//nsp::proto::errorno_t status_;
};