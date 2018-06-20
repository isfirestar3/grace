#include "group_edit.h"
#include "edit_exist_group.h"

group_edit::group_edit(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	tem_manage_ = (template_manage*)(parent);
}

group_edit::~group_edit()
{

}

void group_edit::set_group_data(std::vector<std::string>&group)
{
	std::string data = "";
	for (auto&iter : group){
		data = data + iter + "   ";
	}
	ui.subInfoEdit->setText(QString::fromLocal8Bit(data.c_str()));
}

void group_edit::on_pushButton_clicked()
{
	edit_exist_group edit_page(this);
	edit_page.group_name_ = group_name_;
	edit_page.map_log_types_ = map_log_types_;
	auto iter = template_type_.find(group_name_);
	if (iter != template_type_.end()){
		edit_page.template_type_ = iter->second;
	}
	edit_page.init_edit_group();
	if (edit_page.exec()){
		if (edit_page.is_delete){
			auto iter = template_type_.find(group_name_);
			if (iter != template_type_.end()){
				template_type_.erase(iter);
			}
			tem_manage_->template_type_ = template_type_;
			tem_manage_->ui.tabWidget->removeTab(index);
		}
		else{
			map_log_types_ = edit_page.map_log_types_;
			auto iter = template_type_.find(group_name_);
			if (iter != template_type_.end()){
				group_name_ = edit_page.group_name_;
				template_type_.erase(iter);
				template_type_.insert(std::make_pair(group_name_, edit_page.template_type_));
			}
			tem_manage_->template_type_ = template_type_;
			/*std::string data = "";
			for (auto&editer : edit_page.template_type_){
				data = data + editer + "   ";
			}
			ui.subInfoEdit->setText(QString::fromLocal8Bit(data.c_str()));*/
		}
		tem_manage_->init_page();
	}
}