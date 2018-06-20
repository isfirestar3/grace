#pragma once

#include <QtWidgets\qlabel.h>
#include <QtWidgets\qcombobox.h>
#include <QtWidgets\qsplitter.h>
#include <QtWidgets\qstackedwidget.h>
#include <QtWidgets\qlistview.h>
#include <QtWidgets\qgridlayout.h>
#include <QtCore\QStringListModel>
#include <QtCore\QModelIndex>
#include "push_button.h"
//#include "wid_network_setting.h"
#include "othersettingwidget.h"
#include <functional>
#include <QtCore\QTimer>
#include "networksettingwidget.h"

#define NET_SETTING		0
#define OTHER_SETTING	1

class wid_setting : public QWidget{
	Q_OBJECT
	
public:
	wid_setting(QWidget* parent = 0);
	~wid_setting();

	NetworkSettingWidget *get_wid_network_setting() const
	{
		return net_setting_;
	}

	void set_setiing_callback(const std::function<void(int,int)>& func);
	void AfterShow();
private:
	void init_form();
	void init_slot();

	void update_netstatus(int net_status,int robot_id);

private slots:
	void list_clicked(const QModelIndex&);

private:
	QStackedWidget* update_stackwidget_;
	QSplitter* splitter_;
	QListView* list_view_;
	//wid_network_setting* net_setting_;
	NetworkSettingWidget* net_setting_;
	QWidget*other;
	QSharedPointer<OtherSettingWidget> m_otherSetWidget;

	std::function<void(int,int)> status_function_;
};