#include "group_tab.h"

group_tab::group_tab(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	parent_page_ = (log_select*)(parent);
}

group_tab::~group_tab()
{

}

void group_tab::set_group_data(std::vector<std::string>&group)
{
	std::string data="";
	for (auto&iter : group){
		data = data + iter + "   ";
	}
	ui.subInfoEdit->setText(QString::fromLocal8Bit(data.c_str()));
}

void group_tab::set_group_check(bool state)
{
	ui.radioButton->setChecked(state);
}

void group_tab::on_radioButton_clicked(bool state)
{
	parent_page_->select_group_ = group_name_;
}
