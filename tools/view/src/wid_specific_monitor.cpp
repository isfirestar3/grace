#include "wid_specific_monitor.h"
#include <iostream>

wid_specific_monitor::wid_specific_monitor(QWidget* parent)
:pa{QSharedPointer<QPropertyAnimation>::create()}
{
	init_form();
	QTimer::singleShot(0, this, &wid_specific_monitor::after_show);
}

wid_specific_monitor::~wid_specific_monitor()
{
	//reset_net();
}

void wid_specific_monitor::init_form()
{
	realtime_navigation_ = new wid_realtime_navigation(this);
	realtime_vehicle_ = new wid_realtime_vehicle(this);
	realtime_operation_ = new wid_realtime_operation(this);
	realtime_optpar_ = new wid_realtime_optpar(this);

	realtime_tab_ = new QTabWidget;
	realtime_tab_->addTab(realtime_navigation_, QStringLiteral("导航实时监控"));
	realtime_tab_->addTab(realtime_vehicle_, QStringLiteral("底盘实时监控"));
	realtime_tab_->addTab(realtime_operation_, QStringLiteral("operation实时监控"));
	realtime_tab_->addTab(realtime_optpar_, QStringLiteral("optpar实时监控"));

	connect(realtime_tab_, &QTabWidget::currentChanged, this, &wid_specific_monitor::on_tab_index_changed);

	QVBoxLayout *v_layout = new QVBoxLayout;
	v_layout->addWidget(realtime_tab_);
	setLayout(v_layout);

	QPalette paltter;
	paltter.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setAutoFillBackground(true);
	this->setPalette(paltter);
}

void wid_specific_monitor::set_net_status()
{
	if (nullptr != realtime_navigation_)
	{
		realtime_navigation_->set_net_status(__net_status, robot_id_);
	}
	
	if (nullptr != realtime_vehicle_)
	{
		realtime_vehicle_->set_net_status(__net_status, robot_id_);
	}

	if (nullptr != realtime_operation_)
	{
		realtime_operation_->set_net_status(__net_status, robot_id_);
	}
	
	if (nullptr != realtime_optpar_)
	{
		realtime_optpar_->set_net_status(__net_status, robot_id_);
	}
}

void wid_specific_monitor::register_rece_callback(uint32_t id, void *data, int type)
{
	/*if (type == kNET_STATUS)
	{
		__net_status = *(net_status_t*)data;*/
		set_net_status();
	//}
}

void wid_specific_monitor::on_tab_index_changed(int index)
{
	if (nullptr == realtime_tab_ || pa.isNull())
	{
		return;
	}

	QWidget *pWidget = realtime_tab_->widget(index);

	if (nullptr == pWidget)
	{
		return;
	}

	pa->setTargetObject(pWidget);
	pa->setPropertyName("geometry");
	pa->setDuration(250);
	pa->setStartValue(QRect(-pWidget->width(), 0, pWidget->width(), pWidget->height()));
	pa->setEndValue(pWidget->rect());
	pa->start();
}

void wid_specific_monitor::update_net_status(int status,int robot_id){
	__net_status = status;
	robot_id_ = robot_id;
	set_net_status();
}

void wid_specific_monitor::after_show()
{
	if (nullptr == realtime_tab_)
	{
		return;
	}

	int index = realtime_tab_->currentIndex();
	on_tab_index_changed(index);
}