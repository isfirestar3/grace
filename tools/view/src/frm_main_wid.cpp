#include "frm_main_wid.h"

frm_main_wid::frm_main_wid(QWidget* parent):
QMainWindow(parent),
m_realtimeMonitorWidget{QSharedPointer<RealtimeMonitorWidget>::create()}
{
	mouse_press = false;
	init_wid();
	init_form();
}

frm_main_wid::~frm_main_wid()
{

}

void frm_main_wid::init_wid()
{
	setMinimumSize(900, 600);
	setWindowFlags(Qt::FramelessWindowHint);
	move((QApplication::desktop()->width() - this->width()) / 2, (QApplication::desktop()->height() - this->height()) / 2);
	
}

void frm_main_wid::init_form()
{
	__wid_tool = new wid_tool();
	__wid_tool->setContentsMargins(0, 0, 0, 0);
	__wid_tool->set_current_wid(std::bind(&frm_main_wid::set_current_wid, this, std::placeholders::_1));
	__wid_tool->set_close_func(std::bind(&frm_main_wid::close_wid, this));
	__wid_tool->set_min_func(std::bind(&frm_main_wid::min_wid, this));
	__wid_tool->set_max_func(std::bind(&frm_main_wid::max_wid, this));
	__wid_tool->set_restore_func(std::bind(&frm_main_wid::restore_wid, this));

	statked_widget = new QStackedWidget();
	QPalette palette;
	palette.setBrush(QPalette::Window, QBrush(Qt::white));
	statked_widget->setPalette(palette);
	statked_widget->setAutoFillBackground(true);
	statked_widget->setContentsMargins(0, 0, 0, 0);
	wid_setting_ = new wid_setting();
	//wid_xml_view_ = new wid_xml_tree_view();
	//statked_widget->addWidget(wid_xml_view_);
	//wid_binding_view_ = new wid_mnt_binding_view();
	//statked_widget->addWidget(wid_binding_view_);
	//wid_realtime_monitor_ = new wid_realtime_monitor();
	//statked_widget->addWidget(wid_realtime_monitor_);
	wid_specific_monitor_ = new wid_specific_monitor();
	statked_widget->addWidget(wid_specific_monitor_);
	statked_widget->addWidget(m_realtimeMonitorWidget.data());
	statked_widget->addWidget(wid_setting_);

	wid_setting_->set_setiing_callback(std::bind(&frm_main_wid::set_net_status_update, this, std::placeholders::_1,std::placeholders::_2));
	connect(wid_setting_->get_wid_network_setting(), &NetworkSettingWidget::NotifyToUpdateConnectState, 
		__wid_tool->GetConnectStateWidget(), &ConnectStateWidget::OnUpdateConnectState);

	__lay_main = new QVBoxLayout();
	__lay_main->addWidget(__wid_tool);
	__lay_main->addWidget(statked_widget);
	__lay_main->setContentsMargins(0, 0, 0, 0);
	__lay_main->setSpacing(0);
	QWidget *main_widget = new QWidget;
	main_widget->setLayout(__lay_main);
	this->setCentralWidget(main_widget);
	this->setWindowFlags(Qt::FramelessWindowHint | Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint);
}

void frm_main_wid::set_current_wid(int &index)
{
	if (nullptr == statked_widget)
	{
		return;
	}

	statked_widget->setCurrentIndex(index);
	//根据切换的页,初始化数据
	__current_wid = index;

	QWidget *pWidget = statked_widget->widget(index);

	if (nullptr == pWidget)
	{
		return;
	}

	if (pWidget == wid_specific_monitor_)
	{
		wid_specific_monitor_->after_show();
	}

	switch (__current_wid)
	{
	case 0:
		//wid_xml_view_->init_tree_data();
		break;
	case 1:
		{
			//all_xml_collection all_xml_info;
			//wid_xml_view_->get_all_xml_data(all_xml_info);
			//wid_binding_view_ ->set_all_xml_data(all_xml_info);
			//wid_binding_view_->init_binding_data();
		}
		break;
	case 2:
	{
		//all_xml_collection all_xml_info;
		//wid_xml_view_->get_all_xml_data(all_xml_info);
		//wid_realtime_monitor_->set_all_xml_data(all_xml_info);
		//wid_realtime_monitor_->init_wid_item_data();
	}
		break;
	case 3:

		break;
	case 4:

		break;
	case 5:
	{

	}
		break;
	default:
		break;
	}
}

void frm_main_wid::close_wid()
{
	close();
}

void frm_main_wid::min_wid()
{
	showMinimized();
}
void frm_main_wid::max_wid()
{
	showMaximized();
}
void frm_main_wid::restore_wid()
{
	showNormal();
}
void frm_main_wid::mousePressEvent(QMouseEvent *event)
{
	//只能是鼠标左键移动和改变大小
	//if (event->button() == Qt::LeftButton)
	//{
		mouse_press = true;
	//}
	//Qt::MouseButton btn = event->button();
	//窗口移动距离
	move_point = event->globalPos() - pos();
}

void frm_main_wid::mouseReleaseEvent(QMouseEvent *)
{
	mouse_press = false;
}

void frm_main_wid::mouseMoveEvent(QMouseEvent *event)
{
	//移动窗口
	if (mouse_press)
	{
		QPoint move_pos = event->globalPos();
		move(move_pos - move_point);
	}
}

void frm_main_wid::set_net_status_update(int status, int robot_id){
	//if (nullptr != wid_realtime_monitor_){
	//	wid_realtime_monitor_->update_net_status(status, robot_id);
	//}
	if (nullptr != wid_specific_monitor_){
		wid_specific_monitor_->update_net_status(status, robot_id);
	}

	if (!m_realtimeMonitorWidget.isNull())
	{
		m_realtimeMonitorWidget->UpdateNetStatus(status, robot_id);
	}
}