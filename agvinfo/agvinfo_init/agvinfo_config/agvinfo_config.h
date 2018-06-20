#ifndef AGVINFO_CONFIG_H
#define AGVINFO_CONFIG_H

#include <QtWidgets/QMainWindow>
#include "ui_agvinfo_config.h"
#include <QtGui\QStandardItemModel>
#include <functional>
#include "libagvinfo.h"
#include <mutex>
#include <QtWidgets>
#include <deque>
#include "edit_attribute.h"
#include <QtCore\QSignalMapper>
#include "edit_attribute_value.h"
#include "agv_single.h"

static void fun_notify();

class agvinfo_config : public QMainWindow
{
	Q_OBJECT
public:
	agvinfo_config(QWidget *parent = 0);
	~agvinfo_config();

public:
	void notify_handler();

private slots:
	void on_updatePushButton_clicked();
	void update_table_view();
	void on_getPushButton_clicked();
	void on_pushButton_clicked();
	void on_combPushButton_clicked();
	void on_morePushButton_clicked();
	void on_backButton_clicked();
	void on_attributeButton_clicked();
	void btn_edit_slot(QString index);
	void on_pushButton_add_clicked();
	void on_pushButton_delete_clicked();
	void on_merge_clicked();

	void sortByColumn(int index);
private:
	void agvinfo_regiset_callback(const std::function<void()>&func);

	int init_agvinfo();

	void cover_local_agvinfo(const agv_info * agvs);

	bool merge(bool isAll = false);

	int find_first_unknow_row(int nBegin = 0);

	int find_first_offline_row(std::string ipstr);

	void update_agvinfo(std::vector<inner_agvinfo> vec_agvinfo);

	void refresh_attr_page();

	void refresh_view_page();

	QTableWidgetItem * setitem(QTableWidget * table, int index, int subindex, QString strtext);
	QTableWidgetItem * setitem(QTableWidget * table, int index, int subindex, int strtext);

	bool insert_row(int row, const inner_agvinfo & agvinfo);
private:
	QStandardItemModel *view_model_;
	Ui::agvinfo_configClass ui;
	std::function<void()>regiset_callback_;//更新agv信息
	std::mutex agv_lock_;
	QSignalMapper* pMapper_;
	std::vector<inner_agvinfo> vec_agvinfo_;
	std::map<int, bool> map_soft_;
	int soft_index_ = 0;
};

#endif // AGVINFO_CONFIG_H
