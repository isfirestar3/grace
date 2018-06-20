#include "wid_realtime_monitor.h"
#include "rapidxml.hpp"
#include "rapidxml_iterators.hpp"
#include "rapidxml_print.hpp"
#include "rapidxml_utils.hpp"
#include <iostream>

wid_realtime_monitor::wid_realtime_monitor(QWidget* parent)
{
	init_form();
}

wid_realtime_monitor::~wid_realtime_monitor()
{
	reset_net();
}

void wid_realtime_monitor::init_form()
{
	QHBoxLayout* h_wid_layout = new QHBoxLayout;
	sub_moniter_page1_ = new wid_sub_realtime_moniter;
	sub_moniter_page2_ = new wid_sub_realtime_moniter;
	sub_moniter_page3_ = new wid_sub_realtime_moniter;
	h_wid_layout->addWidget(sub_moniter_page1_);
	h_wid_layout->addWidget(sub_moniter_page2_);
	h_wid_layout->addWidget(sub_moniter_page3_);
	
	sub_moniter_page1_->set_child_wid(std::bind(&wid_realtime_monitor::child_wid_monitor_id, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	sub_moniter_page2_->set_child_wid(std::bind(&wid_realtime_monitor::child_wid_monitor_id, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
	sub_moniter_page3_->set_child_wid(std::bind(&wid_realtime_monitor::child_wid_monitor_id, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

	QVBoxLayout *v_layout = new QVBoxLayout;
	v_layout->addLayout(h_wid_layout);
	setLayout(v_layout);

	QPalette paltter;
	paltter.setColor(QPalette::Background, QColor(221, 225, 230));
	this->setAutoFillBackground(true);
	this->setPalette(paltter);
}
	
void wid_realtime_monitor::register_rece_callback(uint32_t id, void *data, int type)
{
	switch (type)
	{
	case mn::kNET_STATUS:
		{
			std::cout << "the net is break" << std::endl;
			__net_status = *(mn::net_status_t*)data;
			set_net_status();
		}
		break;
	default:
		break;
	}
}

void wid_realtime_monitor::set_net_status()
{
	sub_moniter_page1_->set_net_status(__net_status, robot_id_);
	sub_moniter_page2_->set_net_status(__net_status, robot_id_);
	sub_moniter_page3_->set_net_status(__net_status, robot_id_);
}

void wid_realtime_monitor::child_wid_monitor_id(int id, void*ptr, std::string& str_data)
{
	if (ptr == sub_moniter_page1_)
	{
		sub_moniter_page2_->set_other_wid_selected_id(id, str_data);
		sub_moniter_page3_->set_other_wid_selected_id(id, str_data);
	}
	else if (ptr == sub_moniter_page2_)
	{
		sub_moniter_page1_->set_other_wid_selected_id(id, str_data);
		sub_moniter_page3_->set_other_wid_selected_id(id, str_data);
	}
	else if (ptr == sub_moniter_page3_)
	{
		sub_moniter_page1_->set_other_wid_selected_id(id, str_data);
		sub_moniter_page2_->set_other_wid_selected_id(id, str_data);
	}
}

void wid_realtime_monitor::set_all_xml_data(all_xml_collection& info)
{
	all_xml_data_ = info;
}

void wid_realtime_monitor::get_all_xml_data(all_xml_collection& info)
{
	info = all_xml_data_;
}

void wid_realtime_monitor::init_wid_item_data()
{
	sub_moniter_page1_->set_all_xml_data(all_xml_data_);
	sub_moniter_page2_->set_all_xml_data(all_xml_data_);
	sub_moniter_page3_->set_all_xml_data(all_xml_data_);
}

void wid_realtime_monitor::update_net_status(int status, int robot_id){
	__net_status = status;
	robot_id_ = robot_id;
	set_net_status();
}