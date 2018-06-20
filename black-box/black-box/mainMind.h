#ifndef MAINMIND_H
#define MAINMIND_H

#include <QtWidgets/QMainWindow>
#include "ui_mainMind.h"
#include <stdint.h>
#include "ProxyModel.h"
#include "ItemModel.h"
#include "file_schedule.h"
#include <atomic>
#include "rw_xml_file.h"
#include "libagvinfo.h"
#include <QButtonGroup>
#include "connect_manage.h"

class log_select : public QMainWindow
{
	Q_OBJECT

public:
	log_select(QWidget *parent = 0);
	~log_select();


	std::string select_group_;//被选中的日志模板
private slots:
	void on_connectButton_clicked();
	void on_selectButton_clicked();
	void on_sendLogButton_clicked();
	void on_disconnectButton_clicked();
	void on_checkBox_clicked(bool state);
	//void on_checkBox_stateChanged(int state);
	void on_selectEdit_textChanged(const QString &arg);
	void on_todayButton_clicked();
	void on_yestButton_clicked();
	void on_orderButton_clicked();
	void update_table_view();
	void on_newTimeBox_clicked();
	void display_schedule();
	void no_files();
	void wrong_fts_port();
	void logs_count_info();
	void change_state();
	void connected_list();
	void change_state_back();
	void itemChanged(QStandardItem *item);
	void closeEvent(QCloseEvent * event);
	void on_allTimeButton_clicked();
	void on_lastTowHourButton_clicked();
	void arrange_column(int column);
	void on_getIpButton_clicked();
	void on_getIpButton2_clicked();
	void on_AllcheckBox_clicked(bool state);
	void on_iplistWidget_itemClicked(QListWidgetItem * item);
	void on_radioButton_clicked();
	void on_radioButton2_clicked();
	void on_connectedListWidget_itemClicked(QListWidgetItem * item);
	void on_checkAllBox_clicked(bool state);
	void on_templateButton_clicked();
	void on_tempButton_clicked();
	void on_userDefButton_clicked();
	//void on_listWidget_itemClicked(QListWidgetItem*item);
private:
	int connect_agv_share(std::string&ip, std::string&port);
	int connect_to_agvs_share(std::map<std::string, id_ep>&select_ep_);
	int get_file_time(std::string& file_path, uint64_t &file_time);
	void init_box();
	void display_log_types(const std::vector<std::string>&log_vct);
	void display_schedule_a(int total_count, int finish_count);
	void connect_state(const int state, const std::string&ip);
	void get_current_time(std::string&current_t);
	void init_log_group();

	std::map<std::string, std::string>input_ep_;
	std::map<std::string, ep>robot_id_;

	ItemModel* sourceModel_;
	ProxyModel* proxyModel_;
	file_schedule *file_schedule_=NULL;
	int count_;
	std::atomic<bool> is_cancel_=false;
	std::atomic<bool> schedule_close_ = false;
	std::atomic<bool> is_warn_ = true;//首次回调，弹框显示总日志文件个数
	int total_count_;
	QList < QPair<bool, QString>>list_;
	int error_code_;
	agv_info *agvs_;
	std::map<std::string, id_ep>select_ep_;//需要连接的的车辆的IP
	std::map<std::string, id_ep>all_agv_ep_;//获取的所有车辆的IP
	std::map<std::string, int>use_ep_;//需要连接的的车辆的IP
	std::map<std::string, std::string>map_log_types_;//日志类型
	std::map<std::string, std::vector<std::string>>template_type_;//日志模板
	std::string broken_ip_;
	std::recursive_mutex list_mutex_;

	Ui::log_selectClass ui;
	QButtonGroup*btn_time_;
	QButtonGroup*sel_group_;
	QButtonGroup*sel_type_;


};

#endif // MAINMIND_H
