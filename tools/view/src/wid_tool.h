#pragma once

#include <QtWidgets\qwidget.h>
#include <QtWidgets\QVBoxLayout>
#include <QtCore\QSignalMapper>
#include "tool_button.h"
#include <functional>
#include "push_button.h"
#include "connectstatewidget.h"

class wid_tool :public QWidget{
	Q_OBJECT

public:
	wid_tool(QWidget *parent = 0);
	~wid_tool();

	ConnectStateWidget* GetConnectStateWidget() const
	{
		return m_connectStateWidget.data();
	}
public:
	void set_current_wid(const std::function<void(int)>&func);
	void set_close_func(const std::function<void()>&func);
	void set_min_func(const std::function<void()>&func);
	void set_max_func(const std::function<void()>&func);
	void set_restore_func(const std::function<void()>&func);

protected:
	//virtual void resizeEvent(QResizeEvent *);
	virtual void mouseDoubleClickEvent(QMouseEvent * event);

private:
	void init_wid();
	void init_tool();
	void init_slots();

private slots:
	void slot_turn_page(QString current_page);
	void slot_close();
	void slot_min();
	void slot_max();

private:

	QList<ToolButton *> __button_list;
	QVBoxLayout *lay_main{ nullptr };
	std::function<void(int)> __reg_set_current_wid;
	PushButton *__min_button{ nullptr }; //最小化
	PushButton *__close_button{ nullptr }; //关闭
	PushButton *__max_button{ nullptr }; //最大化
	QSharedPointer<ConnectStateWidget> m_connectStateWidget;

	std::function<void()> __reg_close;
	std::function<void()>__reg_min;
	std::function<void()>__reg_max;
	std::function<void()>__reg_restore;
	//int __is_max_;
};