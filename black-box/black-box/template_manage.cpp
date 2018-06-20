#include "template_manage.h"
#include "group_edit.h"
#include <QtWidgets\QMessageBox.h>

template_manage::template_manage(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
}

template_manage::~template_manage()
{

}

void template_manage::init_page()
{
	for (auto&iter : map_log_types_){
		QListWidgetItem *item = new QListWidgetItem(QString::fromLocal8Bit(iter.first.c_str()));//列表只显示IP
		item->setCheckState(Qt::Unchecked);
		ui.listWidget->addItem(item);
	}
	int i = 0;
	ui.tabWidget->clear();
	for (auto&iter : template_type_){
		group_edit*tab_page = new group_edit(this);
		tab_page->set_group_data(iter.second);
		tab_page->group_name_ = iter.first;
		tab_page->map_log_types_ = map_log_types_;
		tab_page->template_type_ = template_type_;
		tab_page->index = i;
		ui.tabWidget->insertTab(i, tab_page, QString::fromLocal8Bit(iter.first.c_str()));
		i++;
	}
}

void template_manage::on_pushButton_clicked()
{
	if (ui.lineEdit->text()==""){
		QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("new name is empty！"), QMessageBox::Ok);//没有勾选日志类型
		return;
	}
	int count = 0;
	std::vector<std::string>new_template;
	for (int row = 0; row < ui.listWidget->count(); ++row){
		if (ui.listWidget->item(row)->checkState() == Qt::Checked){
			new_template.push_back(ui.listWidget->item(row)->text().toStdString());
			count++;
		}
	}
	if (ui.positionBox->checkState() == Qt::Checked){//获取定位图
		new_template.push_back("localization");
	}
	if (ui.repositionBox->checkState() == Qt::Checked){//获取二次定位图
		new_template.push_back("deviation");
	}
	if (ui.systemLogBox->checkState() == Qt::Checked){
		new_template.push_back("system_log");
	}
	if (count == 0){
		QMessageBox::information(this, QStringLiteral("warning："), QStringLiteral("select any log type!"), QMessageBox::Ok);//没有勾选日志类型
		return;
	}
	template_type_.insert(std::make_pair(ui.lineEdit->text().toStdString(), new_template));

	//int i = ui.tabWidget->count();
	//group_edit*tab_page = new group_edit(this);
	//tab_page->set_group_data(new_template);
	//tab_page->group_name_ = ui.lineEdit->text().toStdString();
	//tab_page->map_log_types_ = map_log_types_;
	//tab_page->template_type_ = template_type_;
	//ui.tabWidget->insertTab(i + 1, tab_page, ui.lineEdit->text());
	ui.tabWidget->clear();
	int i = 0;
	for (auto&iter : template_type_){
		group_edit*tab_page = new group_edit(this);
		tab_page->set_group_data(iter.second);
		tab_page->group_name_ = iter.first;
		tab_page->map_log_types_ = map_log_types_;
		tab_page->template_type_ = template_type_;
		tab_page->index = i;
		ui.tabWidget->insertTab(i, tab_page, QString::fromLocal8Bit(iter.first.c_str()));
		i++;
	}
}