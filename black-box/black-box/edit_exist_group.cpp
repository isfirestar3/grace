#include "edit_exist_group.h"
//#include <qlistwidget.h>

edit_exist_group::edit_exist_group(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	QRegExp regExp("^([a-z_A-Z-.+0-9]+)$");//数字字母_.+
	
	//QRegExp regExp("^[0-9a-zA_Z]+$");//数字和字母
	ui.lineEdit->setValidator(new QRegExpValidator(regExp, this));
}

edit_exist_group::~edit_exist_group()
{

}

void edit_exist_group::init_edit_group()
{
	ui.listWidget->setEnabled(false);
	ui.positionBox->setEnabled(false);
	ui.repositionBox->setEnabled(false);
	ui.systemLogBox->setEnabled(false);
	ui.lineEdit->setText(QString::fromLocal8Bit(group_name_.c_str()));
	std::string data = "";
	for (auto&iter : template_type_){
		data = data + iter + "   ";
	}
	ui.subInfoEdit->setText(QString::fromLocal8Bit(data.c_str()));

	for (auto&iter : map_log_types_){
		QListWidgetItem *item = new QListWidgetItem(QString::fromLocal8Bit(iter.first.c_str()));//列表只显示IP	
		item->setCheckState(Qt::Unchecked);
		ui.listWidget->addItem(item);
	}	
}

void edit_exist_group::on_okButton_clicked()
{
	if (ui.checkBox->checkState() == Qt::Checked){
		template_type_.clear();
		for (int row = 0; row < ui.listWidget->count(); ++row){
			if (ui.listWidget->item(row)->checkState() == Qt::Checked){
				template_type_.push_back(ui.listWidget->item(row)->text().toStdString());
			}
		}
		if (ui.positionBox->checkState() == Qt::Checked){//获取定位图
			template_type_.push_back("localization");
		}
		if (ui.repositionBox->checkState() == Qt::Checked){//获取二次定位图
			template_type_.push_back("deviation");
		}
		if (ui.systemLogBox->checkState() == Qt::Checked){
			template_type_.push_back("system_log");
		}
	}
	if (ui.lineEdit->text().toStdString() != ""){
		group_name_ = ui.lineEdit->text().toStdString();
	}

}

void edit_exist_group::on_deleteButton_clicked()
{
	is_delete = true;
	this->accept();
}

void edit_exist_group::on_checkBox_clicked(bool state)
{
	if (state){
		ui.listWidget->setEnabled(true); 
		ui.positionBox->setEnabled(true); 
		ui.repositionBox->setEnabled(true);
		ui.systemLogBox->setEnabled(true);
	}
	else{
		ui.listWidget->setEnabled(false);
		ui.positionBox->setEnabled(false);
		ui.repositionBox->setEnabled(false);
		ui.systemLogBox->setEnabled(false);
	}
}