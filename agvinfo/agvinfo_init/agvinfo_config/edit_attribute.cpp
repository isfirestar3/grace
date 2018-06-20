#include "edit_attribute.h"
#include "toolkit.h"
#include "agv_single.h"

edit_attribute::edit_attribute(QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);
	//connect(ui.tableWidget, SIGNAL(itemSelectionChanged()), this, SLOT(item_changed( )));
	//connect(ui.tableWidget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(item_changed(QTableWidgetItem *)));
}

edit_attribute::~edit_attribute()
{

}

void edit_attribute::init_tablewidget()
{
	int row = 0;
	for (auto iter : map_attribute_){
		add_attribute(row,iter.first,false);
		row++;
	}
}

void edit_attribute::add_attribute(int index,std::string strattr, bool bCheck ){
	ui.tableWidget->insertRow(index);
	QCheckBox*check_box = new QCheckBox;
	check_box->setChecked(bCheck);
	ui.tableWidget->setCellWidget(index, 0, check_box);

	QTableWidgetItem* vename = new QTableWidgetItem;
	vename->setText(QString::fromLocal8Bit(strattr.c_str()));
	ui.tableWidget->setItem(index, 1, vename);
	ui.tableWidget->item(index, 1)->setTextAlignment(Qt::AlignHCenter | Qt::AlignVCenter);
}

void edit_attribute::on_addButton_clicked()
{
	add_attribute(ui.tableWidget->rowCount());
}

void edit_attribute::on_deleteButton_clicked()
{
	std::vector<int>del_row;
	for (int row = 0; row < ui.tableWidget->rowCount(); row++){
		QCheckBox *checkBox = (QCheckBox*)ui.tableWidget->cellWidget(row, 0);
		if (checkBox->checkState() == Qt::Checked){
			del_row.insert(del_row.begin(),row);
		}
	}
	for (auto&iter : del_row){
		ui.tableWidget->removeRow(iter);
	}
}

void edit_attribute::on_yesButton_clicked()
{
	std::map<std::string, std::string> map_attr;
	for (int index = 0; index < ui.tableWidget->rowCount(); index++){
		//QCheckBox *checkBox = (QCheckBox*)ui.tableWidget->cellWidget(index, 0);
		//if (checkBox->checkState() == Qt::Checked){
			QTableWidgetItem*vename = ui.tableWidget->item(index, 1);
			if (vename->text() == ""){
				continue;
			}
			auto iter = map_attribute_.find(vename->text().toStdString().c_str());
			if (iter != map_attribute_.end()){
				map_attr.emplace(iter->first, iter->second);
			}
			else{
				map_attr.emplace(vename->text().toStdString(), "");
			}
			//}
	}
	agv_info * agvinfo = nullptr;
	if (!nsp::toolkit::singleton<agv_single>::instance()->build_agvinfo(vec_agvinfo_, [&](int index)->std::map<std::string, std::string>{
		return map_attr;
	}, &agvinfo)){
		if (cover_agvinfo(agvinfo, LAM_Server)){
			QMessageBox::information(this, QStringLiteral("warning£º"), QStringLiteral("failed to update data."), QMessageBox::Ok);
		}
	}
}

void edit_attribute::item_changed(QTableWidgetItem * item){
	QString attr_name = item->text();
	if ("" == attr_name){
		return;
	}

	for (int index = 0; index < ui.tableWidget->rowCount(); index++){
		QTableWidgetItem*vename = ui.tableWidget->item(index, 1);
		if (vename == item){
			continue;
		}
		if (attr_name == vename->text()){
			QTimer::singleShot(10, this, SLOT(set_item_edit()));
			break;
		}
	}
}

void edit_attribute::item_changed( ){
	QTimer::singleShot(10, this, SLOT(set_item_edit()));
	//QString attr_name = item->text();
	//if ("" == attr_name){
	//	return;
	//}

	//for (int index = 0; index < ui.tableWidget->rowCount(); index++){
	//	QTableWidgetItem*vename = ui.tableWidget->item(index, 1);
	//	if (vename == item){
	//		continue;
	//	}
	//	if (attr_name == vename->text()){
	//		QTimer::singleShot(10, this, SLOT(set_item_edit()));
	//		break;
	//	}
	//}
}

void edit_attribute::set_item_edit(){
	ui.tableWidget->setFocus();
	ui.tableWidget->editItem(ui.tableWidget->item(0, 1));
}