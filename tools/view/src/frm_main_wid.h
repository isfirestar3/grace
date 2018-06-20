#pragma once

#include <QtWidgets\QApplication>
#include <QtWidgets\QMainWindow>
#include <QtWidgets\QStackedWidget>
#include <QtWidgets\QDesktopWidget>
#include "wid_realtime_monitor.h"
#include "wid_specific_monitor.h"
#include "wid_tool.h"
#include "wid_setting.h"
#include "realtimemonitorwidget.h"

class frm_main_wid : public QMainWindow
{
	Q_OBJECT
public:
	frm_main_wid(QWidget *parent = nullptr);
	~frm_main_wid();

protected:
	void mousePressEvent(QMouseEvent *event);
	void mouseReleaseEvent(QMouseEvent *event);
	void mouseMoveEvent(QMouseEvent *event);

private:
	void init_wid();
	void init_form();
	void set_current_wid(int &index);
	void close_wid();
	void min_wid();
	void max_wid();
	void restore_wid();

	void set_net_status_update(int status,int robot_id);

private:
	//wid_xml_tree_view *wid_xml_view_{ nullptr };
	//wid_mnt_binding_view *wid_binding_view_{ nullptr };
	//wid_realtime_monitor *wid_realtime_monitor_{ nullptr };
	wid_specific_monitor *wid_specific_monitor_{ nullptr };
	wid_tool * __wid_tool{ nullptr };
	wid_setting *wid_setting_{ nullptr };
	QSharedPointer<RealtimeMonitorWidget> m_realtimeMonitorWidget;
	QStackedWidget *statked_widget{ nullptr };
	int __current_wid = -1;
	QVBoxLayout *__lay_main;
	QPoint move_point; //移动的距离
	bool mouse_press; //按下鼠标左键
};
